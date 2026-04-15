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

	//Debug section


	//End Debug section
	
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

	// Cardinal Direction
	if (bHasInput && GroundSpeed > 3.0f)
	{
		if (!bWasMovingLastFrame)
		{
			// Just started moving - set cardinal direction directly, skip hysteresis
			if (LocomotionAngle >= -45.f && LocomotionAngle < 45.f)
				CardinalDirection = E4CardinalDirection::Front;
			else if (LocomotionAngle >= 45.f && LocomotionAngle < 135.f)
				CardinalDirection = E4CardinalDirection::Right;
			else if (LocomotionAngle >= -135.f && LocomotionAngle < -45.f)
				CardinalDirection = E4CardinalDirection::Left;
			else
				CardinalDirection = E4CardinalDirection::Back;
		}
		else
		{
			CardinalDirection = GetNextCardinalDirection(CardinalDirection, LocomotionAngle);
		}
		bWasMovingLastFrame = true;
	}
	else
	{
		bWasMovingLastFrame = false;
	}
	
	// Reference speed based on blendspace animation authored speeds
	float ReferenceSpeed;
	if (GroundSpeed <= 250.0f)
	{
		ReferenceSpeed = 250.0f;
	}
	else if (GroundSpeed <= 700.0f)
	{
		ReferenceSpeed = FMath::Lerp(250.0f, 700.0f, (GroundSpeed - 250.0f) / 450.0f);
	}
	else
	{
		ReferenceSpeed = FMath::Lerp(700.0f, 1100.0f, FMath::Clamp((GroundSpeed - 700.0f) / 400.0f, 0.0f, 1.0f));
	}

	const float TotalSpeedScale = GroundSpeed / ReferenceSpeed;

	// Play Rate
	{
		const float MinPlayRate = 0.7f;
		const float MaxPlayRate = 1.1f;
		PlayRate = FMath::Clamp(TotalSpeedScale, MinPlayRate, MaxPlayRate);
	}

	// Stride Scale
	{
		StrideScale = (PlayRate > 0.0f) ? TotalSpeedScale / PlayRate : 1.0f;

		/*GEngine->AddOnScreenDebugMessage(3, 0.f, FColor::Yellow,
			FString::Printf(TEXT("Speed=%.1f PlayRate=%.3f StrideScale=%.3f RefSpeed=%.1f"), GroundSpeed, PlayRate, StrideScale, ReferenceSpeed));*/
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

E4CardinalDirection URuneSparkAnimInstance::GetNextCardinalDirection(E4CardinalDirection CurrentDirection, float RelativeDirection,
    float StepDelta, float SkipDelta)
{
    switch (CurrentDirection)
    {
    case E4CardinalDirection::Front:
        {
            if (RelativeDirection > StepDelta)
            {
                return RelativeDirection > SkipDelta ? E4CardinalDirection::Back : E4CardinalDirection::Right;
            }
            else if (RelativeDirection < -StepDelta)
            {
                return RelativeDirection < -SkipDelta ? E4CardinalDirection::Back : E4CardinalDirection::Left;
            }
            break;
        }
    case E4CardinalDirection::Back:
        {
            float OffsetDir = RelativeDirection - 180.0f;
            if (OffsetDir < -180.0f) OffsetDir += 360.0f;
            else if (OffsetDir > 180.0f) OffsetDir -= 360.0f;

            if (OffsetDir > StepDelta)
            {
                return OffsetDir > SkipDelta ? E4CardinalDirection::Front : E4CardinalDirection::Left;
            }
            else if (OffsetDir < -StepDelta)
            {
                return OffsetDir < -SkipDelta ? E4CardinalDirection::Front : E4CardinalDirection::Right;
            }
            break;
        }
    case E4CardinalDirection::Right:
    	{
    		float OffsetDir = RelativeDirection - 90.0f;
    		if (OffsetDir < -180.0f) OffsetDir += 360.0f;

    		const float StrafeStepDelta = 46.0f;
    		const float StrafeToFrontDelta = 70.0f;

    		if (OffsetDir > StrafeStepDelta)
    		{
    			return OffsetDir > SkipDelta ? E4CardinalDirection::Left : E4CardinalDirection::Back;
    		}
    		else if (OffsetDir < -StrafeToFrontDelta)
    		{
    			return OffsetDir < -SkipDelta ? E4CardinalDirection::Back : E4CardinalDirection::Front;
    		}
    		break;
    	}
    case E4CardinalDirection::Left:
    	{
    		float OffsetDir = RelativeDirection + 90.0f;
    		if (OffsetDir > 180.0f) OffsetDir -= 360.0f;

    		const float StrafeStepDelta = 45.0f;
    		const float StrafeToFrontDelta = 70.0f;

    		if (OffsetDir > StrafeToFrontDelta)
    		{
    			return OffsetDir > SkipDelta ? E4CardinalDirection::Right : E4CardinalDirection::Front;
    		}
    		else if (OffsetDir < -StrafeStepDelta)
    		{
    			return OffsetDir < -SkipDelta ? E4CardinalDirection::Right : E4CardinalDirection::Back;
    		}
    		break;
    	}
    default:
        break;
    }

    return CurrentDirection;
}