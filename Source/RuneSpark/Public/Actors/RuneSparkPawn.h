#pragma once

#include "CoreMinimal.h"
#include "RuneSparkMovementComponent.h"
#include "GMCE_MotionWarpingComponent.h"
#include "GMCE_MotionWarpSubject.h"
#include "Actors/GMAS_Pawn.h"
#include "UObject/Object.h"
#include "RuneSparkPawn.generated.h"

class URuneSparkMovementComponent;

/**
 * 
 */
UCLASS()
class RUNESPARK_API ARuneSparkPawn : public AGMAS_Pawn, public IGMCE_MotionWarpSubject
{
	GENERATED_BODY()

public:
	explicit ARuneSparkPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	
	// GMCExtendedAnimation motion warping interface
	virtual USkeletalMeshComponent* MotionWarping_GetMeshComponent() const override;
	virtual float MotionWarping_GetCollisionHalfHeight() const override;
	virtual FQuat MotionWarping_GetRotationOffset() const override;
	virtual FVector MotionWarping_GetTranslationOffset() const override;
	virtual FAnimMontageInstance* GetRootMotionAnimMontageInstance(USkeletalMeshComponent* MeshComponent) const override;
	virtual UGMCE_OrganicMovementCmp* GetGMCExMovementComponent() const override;

protected:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Components")
	URuneSparkMovementComponent* MovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	UGMCE_MotionWarpingComponent* MotionWarpingComponent;


};
