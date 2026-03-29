// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/GMCAbilityAnimInstance.h"
#include "RuneSparkMovementComponent.h"
#include "RuneSparkAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class RUNESPARK_API URuneSparkAnimInstance : public UGMCAbilityAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category="Animation Warping Utility")
	static int32 GetNextCardinalDirection(int32 CurrentCardinalDirection, float RelativeDirection,
		float StepDelta = 60.0f, float SkipDelta = 135.0f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State")
	URuneSparkMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Locomotion")
	int32 CardinalDirection { 0 };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Locomotion")
	float PlayRate { 1.0f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Locomotion")
	float StrideScale { 1.0f };move to new w

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Locomotion")
	float AccelLean { 0.0f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State")
	bool bFirstUpdate { true };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Movement")
	FRotator ActorRotation { FRotator::ZeroRotator };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Movement")
	float ActorYawDeltaRate { 0.f };
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Aiming")
	FRotator AimRotation { FRotator::ZeroRotator };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Aiming")
	float AimYawDeltaRate { 0.f };
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Aiming")
	float AimOffset { 0.f };
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Turn-in-Place")
	bool bIsTurningInPlace { false };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Turn-in-Place")
	float AimYawRemaining { 0.f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Movement")
	FVector WorldVelocity { FVector::ZeroVector };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Movement")
	FVector LocalVelocity { FVector::ZeroVector };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Movement")
	float LocomotionAngle { 0.f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Movement")
	FVector WorldAcceleration { FVector::ZeroVector };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Movement")
	FVector LocalAcceleration { FVector::ZeroVector };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Input")
	bool bHasInput { false };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Input")
	FVector LocalInputDirection { FVector::ZeroVector };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Input")
	FVector LocalInputAcceleration { FVector::ZeroVector };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Input")
	FVector WorldInputDirection { FVector::ZeroVector };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Input")
	FVector WorldInputAcceleration { FVector::ZeroVector };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Distance Matching")
	bool bStopPredicted { false };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Distance Matching")
	float PredictedStopDistance { 0.f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Distance Matching")
	bool bPivotPredicted { false };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Distance Matching")
	float PredictedPivotDistance { 0.f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Movement")
	float GroundSpeed { 0.f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Movement")
	bool bIsMoving { false };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Movement")
	bool bIsGrounded { true };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Movement")
	bool bIsAirborne { false };

private:
	void ObtainComponents();
	
};

