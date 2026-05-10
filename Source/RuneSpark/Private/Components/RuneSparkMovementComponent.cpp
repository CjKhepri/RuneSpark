#include "RuneSparkMovementComponent.h"

URuneSparkMovementComponent::URuneSparkMovementComponent()
{
	bOrientToControlRotationDirection = false;
	bOrientToVelocityDirection = false;
	bOrientToInputDirection = false;
}

void URuneSparkMovementComponent::CalculateVelocity(float DeltaSeconds)
{
	const FVector VelBefore = FVector(Velocity.X, Velocity.Y, 0.f);
	const float SpeedBefore = VelBefore.Size();
	const FVector DirBefore = SpeedBefore > 1.f ? VelBefore / SpeedBefore : FVector::ZeroVector;

	Super::CalculateVelocity(DeltaSeconds);

	if (!IsMovingOnGround())
	{
		return;
	}

	const FVector VelAfter = FVector(Velocity.X, Velocity.Y, 0.f);
	const float SpeedAfter = VelAfter.Size();
	const FVector Input = GetProcessedInputVector();

	if (Input.IsNearlyZero() || DirBefore.IsNearlyZero())
	{
		return;
	}

	const float DirectionDelta = 1.0f - FVector::DotProduct(DirBefore, VelAfter.GetSafeNormal());

	if (DirectionDelta < 0.01f && SpeedAfter > SpeedBefore)
	{
		// Straight line - throttle speed gain
		const float SpeedGain = SpeedAfter - SpeedBefore;
		const float MaxGainPerFrame = StraightLineAcceleration * DeltaSeconds;

		if (SpeedGain > MaxGainPerFrame)
		{
			const float ClampedSpeed = SpeedBefore + MaxGainPerFrame;
			Velocity = VelAfter.GetSafeNormal() * ClampedSpeed + FVector(0.f, 0.f, Velocity.Z);
		}
	}

	// If velocity opposes input, throttle the total velocity change
	const float VelInputAlignment = FVector::DotProduct(DirBefore, Input.GetSafeNormal2D());
	if (VelInputAlignment < 0.0f)
	{
		const FVector VelDelta = VelAfter - VelBefore;
		const float DeltaMagnitude = VelDelta.Size();
		const float MaxDeltaPerFrame = ReversalAcceleration * DeltaSeconds;

		if (DeltaMagnitude > MaxDeltaPerFrame)
		{
			const FVector ClampedDelta = VelDelta.GetSafeNormal() * MaxDeltaPerFrame;
			Velocity = FVector(VelBefore.X + ClampedDelta.X, VelBefore.Y + ClampedDelta.Y, Velocity.Z);
		}
	}
}

FVector URuneSparkMovementComponent::PreProcessInputVector_Implementation(FVector InRawInputVector)
{
	FVector Result = Super::PreProcessInputVector_Implementation(InRawInputVector);

	if (!Result.IsNearlyZero())
	{
		SmoothedInputVector = FMath::VInterpTo(SmoothedInputVector, Result, GetWorld()->GetDeltaSeconds(), 15.0f);
		return SmoothedInputVector;
	}
	else
	{
		SmoothedInputVector = FVector::ZeroVector;
		return Result;
	}
}

void URuneSparkMovementComponent::BindReplicationData_Implementation()
{
	Super::BindReplicationData_Implementation();

	AbilitySystemComponent = Cast<UGMC_AbilitySystemComponent>(GetGMCPawnOwner()->GetComponentByClass(UGMC_AbilitySystemComponent::StaticClass()));
	MotionWarpingComponent = Cast<UGMCE_MotionWarpingComponent>(GetGMCPawnOwner()->GetComponentByClass(UGMCE_MotionWarpingComponent::StaticClass()));

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GMCMovementComponent = this;
		AbilitySystemComponent->BindReplicationData();
	}
}

void URuneSparkMovementComponent::GenAncillaryTick_Implementation(float DeltaTime, bool bLocalMove,
	bool bCombinedClientMove)
{
	Super::GenAncillaryTick_Implementation(DeltaTime, bLocalMove, bCombinedClientMove);

	if (AbilitySystemComponent) AbilitySystemComponent->GenAncillaryTick(DeltaTime, bCombinedClientMove);
}

void URuneSparkMovementComponent::GenPredictionTick_Implementation(float DeltaTime)
{
	Super::GenPredictionTick_Implementation(DeltaTime);

	if (AbilitySystemComponent) AbilitySystemComponent->GenPredictionTick(DeltaTime);
}

void URuneSparkMovementComponent::GenSimulationTick_Implementation(float DeltaTime)
{
	Super::GenSimulationTick_Implementation(DeltaTime);

	if (AbilitySystemComponent) AbilitySystemComponent->GenSimulationTick(DeltaTime);
}

void URuneSparkMovementComponent::PreLocalMoveExecution_Implementation(const FGMC_Move& LocalMove)
{
	Super::PreLocalMoveExecution_Implementation(LocalMove);

	if (AbilitySystemComponent) AbilitySystemComponent->PreLocalMoveExecution();
}

void URuneSparkMovementComponent::OnSolverChangedMode(FGameplayTag NewMode, FGameplayTag OldMode)
{
	Super::OnSolverChangedMode(NewMode, OldMode);

	if (AbilitySystemComponent)
	{
		// Put our active solver into GMAS as an active tag, to make it easier to key abilities off of.
		if (OldMode != FGameplayTag::EmptyTag)
		{
			AbilitySystemComponent->RemoveActiveTag(OldMode);
		}

		if (NewMode != FGameplayTag::EmptyTag)
		{
			AbilitySystemComponent->AddActiveTag(NewMode);
		}
	}

}


void URuneSparkMovementComponent::MovementUpdate_Implementation(float DeltaSeconds)
{
	Super::MovementUpdate_Implementation(DeltaSeconds);

	if (bTurnInPlaceActive && DeltaSeconds > 0.f)
	{
		FVector TargetDirection = TurnInPlaceTargetRotation.Vector();
		RotateYawTowardsDirection(TargetDirection, TurnInPlaceInterpSpeed, DeltaSeconds);
	}
}