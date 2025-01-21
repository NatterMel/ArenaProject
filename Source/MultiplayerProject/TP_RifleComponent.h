// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TP_WeaponComponent.h"
#include "TP_RifleComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MULTIPLAYERPROJECT_API UTP_RifleComponent : public UTP_WeaponComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Rifle")
	TEnumAsByte<ECollisionChannel> TraceChannelProperty = ECC_Pawn;

	UPROPERTY(EditDefaultsOnly, Category = "Rifle")
	float FireRate;

	FTimerHandle FiringTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<class UDamageShot> DamageType;

	UWorld* World;

	FHitResult Hit;

	
public:
	virtual void AttachWeapon(AMultiplayerProjectCharacter* TargetCharacter) override;


protected:
	virtual void Fire() override;

	UFUNCTION(Server, Reliable)
	void ServerPerformLineTrace(FVector Direction);

	virtual void ActivateWeapon() override;

	virtual void Reload() override;

	virtual void GainAmmo() override;
	
};
