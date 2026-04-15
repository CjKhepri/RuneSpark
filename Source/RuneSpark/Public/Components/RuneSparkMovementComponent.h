#pragma once

#include "CoreMinimal.h"
#include "GMCAbilityComponent.h"
#include "GMCE_MotionWarpingComponent.h"
#include "GMCE_OrganicMovementCmp.h"
#include "UObject/Object.h"
#include "RuneSparkMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class RUNESPARK_API URuneSparkMovementComponent : public UGMCE_OrganicMovementCmp
{
	GENERATED_BODY()

public:
	
	URuneSparkMovementComponent();

	virtual FVector PreProcessInputVector_Implementation(FVector InRawInputVector) override;

	FVector SmoothedInputVector = FVector::ZeroVector;

	virtual void CalculateVelocity(float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Tempo")
	float DirectionChangeAcceleration = 6000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Tempo")
	float StraightLineAcceleration = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Tempo")
	float ReversalAcceleration = 3000.0f;
	
	// GMAS overrides
	virtual void BindReplicationData_Implementation() override;
	virtual void GenAncillaryTick_Implementation(float DeltaTime, bool bLocalMove, bool bCombinedClientMove) override;
	virtual void GenPredictionTick_Implementation(float DeltaTime) override;
	virtual void GenSimulationTick_Implementation(float DeltaTime) override;
	virtual void PreLocalMoveExecution_Implementation(const FGMC_Move& LocalMove) override;

	// GMCEx overrides
	virtual void OnSolverChangedMode(FGameplayTag NewMode, FGameplayTag OldMode) override;

protected:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Components")
	/// Convenience reference to the GMAS ability system component.
	UGMC_AbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Components")
	/// Convenience reference to the GMCEx motion warping component.
	UGMCE_MotionWarpingComponent* MotionWarpingComponent;
	
};
