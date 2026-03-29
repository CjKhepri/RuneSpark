// Fill out your copyright notice in the Description page of Project Settings.


#include "RuneSparkAnimInstance.h"

void URuneSparkAnimInstance::ObtainComponents()
{
	if (!IsValid(GMCPawn) || !IsValid(MovementComponent))
	{
		
#if WITH_EDITOR
		if (!GetWorld()->IsGameWorld())
		{
			// If we're an editor preview, just make sure we have valid data so we don't, y'know, crash.
			if (!GMCPawn) GMCPawn = EditorPreviewClass->GetDefaultObject<AGMC_Pawn>();
			MovementComponent = Cast<URuneSparkMovementComponent>(GMCPawn->GetMovementComponent());

			if (!MovementComponent)
			{
				MovementComponent = NewObject<URuneSparkMovementComponent>();
			}

			return;
		}
#endif
		
		if (!GMCPawn)
		{
			AActor* OwningActor = GetOwningActor();
			GMCPawn = Cast<AGMC_Pawn>(OwningActor);
		}

		if (GMCPawn && !MovementComponent)
		{
			MovementComponent = Cast<URuneSparkMovementComponent>(GMCPawn->GetComponentByClass<UGMCE_OrganicMovementCmp>());
		}
	}	
}


void URuneSparkAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ObtainComponents();	
}

void URuneSparkAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	// We run this here just in case components were added in blueprint and weren't available in initialize animation.
	ObtainComponents();
}

void URuneSparkAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (bFirstUpdate && !MovementComponent && GMCPawn)
	{
		MovementComponent = Cast<URuneSparkMovementComponent>(GMCPawn->GetMovementComponent());
	}

	// No component, nothing we can do.
	if (!MovementComponent) return;

	WorldVelocity = MovementComponent->GetLinearVelocity_GMC();
	WorldAcceleration = MovementComponent->GetCurrentAnimationAcceleration();

	AimRotation = MovementComponent->GetCurrentAimRotation();
	AimYawDeltaRate = MovementComponent->GetCurrentAimYawRate();

	ActorRotation = MovementComponent->GetCurrentComponentRotation();
	ActorYawDeltaRate = MovementComponent->GetCurrentComponentYawRate();

	AimYawRemaining = MovementComponent->GetAimYawRemaining();
	
	const FVector InputVector = MovementComponent->GetRawInputVector();
	const FVector InputDirection = InputVector.GetSafeNormal();
	const FVector InputAcceleration = InputVector * MovementComponent->GetInputAcceleration();

	bIsTurningInPlace = (WorldVelocity.IsNearlyZero() && AimYawRemaining != 0.f);

	bHasInput = MovementComponent->IsInputPresent();

	FVector Prediction;
	bStopPredicted = MovementComponent->IsStopPredicted(Prediction);
	PredictedStopDistance = bStopPredicted ? Prediction.Length() : 0.f;

	bPivotPredicted = MovementComponent->IsPivotPredicted(Prediction);
	PredictedPivotDistance = bPivotPredicted ? Prediction.Length() : 0.f;

	if (GetWorld()->IsGameWorld())
	{
		const auto ComponentTransform = GMCPawn->GetActorTransform();
		LocalVelocity = ComponentTransform.InverseTransformVector(WorldVelocity);
		LocalAcceleration = ComponentTransform.InverseTransformVector(WorldAcceleration);
		WorldInputDirection = MovementComponent->GetControllerRotation_GMC().RotateVector(InputDirection);
		WorldInputAcceleration = MovementComponent->GetControllerRotation_GMC().RotateVector(InputAcceleration);
		LocalInputDirection = ComponentTransform.InverseTransformVector(WorldInputDirection);
		LocalInputAcceleration = ComponentTransform.InverseTransformVector(WorldInputAcceleration);
	}
	else
	{
		// We're an editor preview, there's no actual world transform to bother with.
		LocalVelocity = WorldVelocity;
		LocalAcceleration = WorldAcceleration;
		WorldInputDirection = LocalInputDirection = InputDirection;
		WorldInputAcceleration = LocalInputAcceleration = InputAcceleration;
	}

	// Ground speed (horizontal only)
	GroundSpeed = FVector(WorldVelocity.X, WorldVelocity.Y, 0.f).Size();
	bIsMoving = GroundSpeed > 3.0f && bHasInput;

	// Locomotion angle - direction of movement relative to actor facing
	if (GroundSpeed > 3.0f)
	{
		LocomotionAngle = MovementComponent->GetLocomotionAngle();
	}

	// Movement mode
	bIsAirborne = MovementComponent->IsAirborne();
	bIsGrounded = !bIsAirborne;

	// Cardinal Direction (only update when moving)
	if (bHasInput)
	{
		CardinalDirection = GetNextCardinalDirection(CardinalDirection, LocomotionAngle * 1.2f);
	}

	// Play Rate
	{
		const float TotalSpeedScale = GroundSpeed / 700.0f;
		const float PlaybackWeight = 0.5f;
		const float MinPlayRate = 0.9f;
		const float MaxPlayRate = 1.1f;

		const float WeightedSpeed = FMath::Lerp(1.0f, TotalSpeedScale, PlaybackWeight);
		PlayRate = FMath::Clamp(WeightedSpeed, MinPlayRate, MaxPlayRate);
	}

	// Stride Scale
	{
		const float TotalSpeedScale = GroundSpeed / 700.0f;
		StrideScale = (PlayRate > 0.0f) ? TotalSpeedScale / PlayRate : 1.0f;
	}

	// Accel Lean
	{
		const float SpeedOverBase = FMath::Clamp(GroundSpeed - 700.0f, 0.0f, 400.0f);
		const float DotProduct = FVector::DotProduct(
			LocalVelocity.GetSafeNormal(),
			LocalAcceleration.GetSafeNormal());
		AccelLean = FMath::Lerp(0.0f, SpeedOverBase, DotProduct);
	}
	
	bFirstUpdate = false;
}

int32 URuneSparkAnimInstance::GetNextCardinalDirection(int32 CurrentCardinalDirection, float RelativeDirection,
	float StepDelta, float SkipDelta)
{
	switch (CurrentCardinalDirection)
	{
	case 0: // North
		{
			if (RelativeDirection > StepDelta)
			{
				return RelativeDirection > SkipDelta ? 2 : 1;
			}
			else if (RelativeDirection < -StepDelta)
			{
				return RelativeDirection < -SkipDelta ? 2 : 3;
			}
		}
		break;

	case 1: // East
		{
			float OffsetDir = RelativeDirection - 90.0f;
			if (OffsetDir < -180.0f) OffsetDir += 360.0f;

			if (OffsetDir > StepDelta)
			{
				return OffsetDir > SkipDelta ? 3 : 2;
			}
			else if (OffsetDir < -StepDelta)
			{
				return OffsetDir < -SkipDelta ? 3 : 0;
			}
		}
		break;

	case 2: // South
		{
			float OffsetDir = RelativeDirection - 180.0f;
			if (OffsetDir < -180.0f) OffsetDir += 360.0f;
			else if (OffsetDir > 180.0f) OffsetDir -= 360.0f;

			if (OffsetDir > StepDelta)
			{
				return OffsetDir > SkipDelta ? 0 : 3;
			}
			else if (OffsetDir < -StepDelta)
			{
				return OffsetDir < -SkipDelta ? 0 : 1;
			}
		}
		break;

	case 3: // West
		{
			float OffsetDir = RelativeDirection + 90.0f;
			if (OffsetDir > 180.0f) OffsetDir -= 360.0f;

			if (OffsetDir > StepDelta)
			{
				return OffsetDir > SkipDelta ? 1 : 0;
			}
			else if (OffsetDir < -StepDelta)
			{
				return OffsetDir < -SkipDelta ? 1 : 2;
			}
		}
		break;
	default:
		break;
	}

	return CurrentCardinalDirection;
}