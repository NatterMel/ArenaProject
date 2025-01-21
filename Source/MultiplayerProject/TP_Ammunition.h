// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "TP_Ammunition.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MULTIPLAYERPROJECT_API UTP_Ammunition : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void CallAmmo(AMultiplayerProjectCharacter* TargetCharacter);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawner")
	UTP_WeaponComponent* WeaponToAdd;
};
