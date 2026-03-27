#include "RuneSparkPawn.h"

#include "EnhancedInputComponent.h"
#include "RuneSpark/Components/RuneSparkMovementComponent.h"
#include "Utility/GMASUtilities.h"

ARuneSparkPawn::ARuneSparkPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MotionWarpingComponent = ObjectInitializer.CreateDefaultSubobject<UGMCE_MotionWarpingComponent>(this, TEXT("MotionWarpingComponent"));
	OverrideInputComponentClass = UEnhancedInputComponent::StaticClass();
}

void ARuneSparkPawn::BeginPlay()
{
	Super::BeginPlay();
	MovementComponent = GetComponentByClass<URuneSparkMovementComponent>();

	if (!IsValid(MovementComponent))
	{
		UE_LOG(LogActor, Error, TEXT("%s has no valid movement component!"), *GetName());
	}
}

USkeletalMeshComponent* ARuneSparkPawn::MotionWarping_GetMeshComponent() const
{
	return MeshComponent;
}

float ARuneSparkPawn::MotionWarping_GetCollisionHalfHeight() const
{
	if (!MovementComponent) return 0.f;
	
	return MovementComponent->GetRootCollisionHalfHeight(true);
}

FQuat ARuneSparkPawn::MotionWarping_GetRotationOffset() const
{
	if (!MeshComponent) return FQuat::Identity;
	
	return MeshComponent->GetRelativeRotation().Quaternion();
}

FVector ARuneSparkPawn::MotionWarping_GetTranslationOffset() const
{
	if (!MeshComponent) return FVector::ZeroVector;
	
	return MeshComponent->GetRelativeLocation();
}

FAnimMontageInstance* ARuneSparkPawn::GetRootMotionAnimMontageInstance(USkeletalMeshComponent* InMeshComponent) const
{
	return IGMCE_MotionWarpSubject::GetRootMotionAnimMontageInstance(InMeshComponent);
}

UGMCE_OrganicMovementCmp* ARuneSparkPawn::GetGMCExMovementComponent() const
{
	return MovementComponent;
}
