#include "Actors/RuneSparkPawn.h"

#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "Components/RuneSparkMovementComponent.h"


ARuneSparkPawn::ARuneSparkPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MovementComponent = ObjectInitializer.CreateDefaultSubobject<URuneSparkMovementComponent>(this, TEXT("MovementComponent"));
	MotionWarpingComponent = ObjectInitializer.CreateDefaultSubobject<UGMCE_MotionWarpingComponent>(this, TEXT("MotionWarpingComponent"));
	OverrideInputComponentClass = UEnhancedInputComponent::StaticClass();

	// Configure the parent's spring arm for third-person shooter
	if (SpringArmComponent)
	{
		SpringArmComponent->SetupAttachment(MeshComponent);
		SpringArmComponent->TargetArmLength = 350.0f;
		SpringArmComponent->bUsePawnControlRotation = true;
		SpringArmComponent->bEnableCameraLag = false;
		SpringArmComponent->CameraLagSpeed = 10.0f;
		SpringArmComponent->SocketOffset = FVector(0.f, 0.f, 0.f);
	}
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

void ARuneSparkPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (JumpAction)
		{
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ARuneSparkPawn::HandleJumpPressed);
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ARuneSparkPawn::HandleJumpReleased);
		}
	}
}

void ARuneSparkPawn::HandleJumpPressed()
{
	// Movement layer implementation coming
}

void ARuneSparkPawn::HandleJumpReleased()
{
	// Movement layer implementation coming
}

// --- Motion Warping Interface ---

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