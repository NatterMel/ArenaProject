// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TP_WeaponComponent.h"
#include "TP_Rocket.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MULTIPLAYERPROJECT_API UTP_Rocket : public UTP_WeaponComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Rocket")
	float FireRate;

	UPROPERTY(EditDefaultsOnly, Category = "Rocket")
	float DamageRadius;

	FTimerHandle FiringTimer;

	UPROPERTY(EditDefaultsOnly, Category = "Rocket")
	TSubclassOf<class ARocketProjectile> ProjectileClass;

	UWorld* World;

public:
	virtual void AttachWeapon(AMultiplayerProjectCharacter* TargetCharacter) override;

	UFUNCTION(Server, Reliable)
	void SpawnProjectile(FVector Direction);

	virtual void GainAmmo() override;

protected:
	virtual void Fire() override;

	virtual void ActivateWeapon() override;

	virtual void Reload() override;

};
