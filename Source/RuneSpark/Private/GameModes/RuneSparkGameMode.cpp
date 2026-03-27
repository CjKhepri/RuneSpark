#include "GameModes/RuneSparkGameMode.h"
#include "Actors/RuneSparkPawn.h"
#include "Controllers/RuneSparkPlayerController.h"

ARuneSparkGameMode::ARuneSparkGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PawnBPClass(TEXT("/Game/RuneSpark/Blueprints/BP_RuneSparkPawn"));
	if (PawnBPClass.Succeeded())
	{
		DefaultPawnClass = PawnBPClass.Class;
	}

	PlayerControllerClass = ARuneSparkPlayerController::StaticClass();
}