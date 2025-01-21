// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerProjectGameMode.h"
#include "MultiplayerProjectCharacter.h"
#include "MultiplayerProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

AMultiplayerProjectGameMode::AMultiplayerProjectGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}

void AMultiplayerProjectGameMode::RestartPlayer(AController* Player)
{
	Super::RestartPlayer(Player);
	APawn* Pawn = Player->GetPawn();
	AMultiplayerProjectCharacter* Char = Cast<AMultiplayerProjectCharacter>(Pawn);
	if(Char != nullptr)
	{
		UMaterialInterface* Mat = MaterialsArray[Count];
		if (Count >= MaterialsArray.Num())
		{
			return;
		}
		if(Mat != nullptr)
		{
			Char->SetMat(Mat);
			++Count;
		}

	}
}

void AMultiplayerProjectGameMode::RespawnPlayer(AController* Player, UMaterialInterface* Material)
{
    if (Player)
    {
        FVector SpawnLocation;
        FRotator SpawnRotation;
        bool bFoundSpawn = false;

        // Get all PlayerStart actors in the level
        TArray<AActor*> PlayerStarts;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

        if (PlayerStarts.Num() > 0)
        {
            int32 RandomIndex = FMath::RandRange(0, PlayerStarts.Num() - 1);
            APlayerStart* ChosenStart = Cast<APlayerStart>(PlayerStarts[RandomIndex]);

            if (ChosenStart)
            {
                SpawnLocation = ChosenStart->GetActorLocation();
                SpawnRotation = ChosenStart->GetActorRotation();
                bFoundSpawn = true;
            }
        }

        if (bFoundSpawn)
        {
            // Spawn a new pawn for the player
            APawn* NewPawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, SpawnLocation, SpawnRotation);

            if (NewPawn)
            {
                // Possess the new pawn
                Player->Possess(NewPawn);

                // Set the material
                AMultiplayerProjectCharacter* Char = Cast<AMultiplayerProjectCharacter>(NewPawn);
                if (Char)
                {
                    Char->SetMat(Material);
                }
            }
        }
    }
}


