// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MultiplayerProjectGameMode.generated.h"

UCLASS(minimalapi)
class AMultiplayerProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMultiplayerProjectGameMode();
	void RespawnPlayer(AController* Player, UMaterialInterface* Material);
protected:
	UPROPERTY(EditAnywhere)
	TArray<UMaterialInterface*> MaterialsArray;

	float Count;

	virtual void RestartPlayer(AController* Player) override;



};



