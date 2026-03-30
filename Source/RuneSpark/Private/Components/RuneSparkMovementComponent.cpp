#include "RuneSparkMovementComponent.h"

URuneSparkMovementComponent::URuneSparkMovementComponent()
{
	bOrientToControlRotationDirection = true;
	bOrientToVelocityDirection = false;
	bOrientToInputDirection = false;
}

void URuneSparkMovementComponent::CalculateVelocity(float DeltaSeconds)
{
	Super::CalculateVelocity(DeltaSeconds);

	if (!IsMovingOnGround() || Velocity.IsNearlyZero())
	{
		return;
	}

	const FVector Input = GetProcessedInputVector();
	if (Input.IsNearlyZero())
	{
		return;
	}

	const FVector VelDir = Velocity.GetSafeNormal2D();
	const FVector InputDir = Input.GetSafeNormal2D();
	const float Alignment = FVector::DotProduct(VelDir, InputDir);

	if (Alignment < 0.95f)
	{
		// Push velocity toward input direction
		const float BlendAlpha = FMath::Clamp(1.0f - Alignment, 0.0f, 1.0f);
		const float ExtraAccel = DirectionChangeAcceleration * BlendAlpha * DeltaSeconds;
		Velocity += InputDir * ExtraAccel;

		// Scrub velocity perpendicular to input direction
		const FVector VelAlongInput = InputDir * FVector::DotProduct(Velocity, InputDir);
		const FVector VelPerpendicular = Velocity - FVector(0.f, 0.f, Velocity.Z) - VelAlongInput;
		Velocity -= VelPerpendicular * FMath::Clamp(DirectionChangeScrubRate * DeltaSeconds, 0.f, 1.f);

		// Don't exceed max speed
		const float MaxSpeed = GetMaxSpeed();
		if (Velocity.Size2D() > MaxSpeed)
		{
			Velocity = Velocity.GetSafeNormal2D() * MaxSpeed + FVector(0.f, 0.f, Velocity.Z);
		}
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
