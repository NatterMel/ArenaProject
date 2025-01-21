// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TP_WeaponComponent.h"
#include "TP_Shotgun.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MULTIPLAYERPROJECT_API UTP_Shotgun : public UTP_WeaponComponent
{
	GENERATED_BODY()


protected: 

	UPROPERTY(EditAnywhere, Category = "Shotgun")
	TEnumAsByte<ECollisionChannel> TraceChannelProperty = ECC_Pawn;

	UPROPERTY(EditDefaultsOnly, Category = "Shotgun")
	float Spread;

	UPROPERTY(EditDefaultsOnly, Category = "Shotgun")
	float FireRate;

	FTimerHandle FiringTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<class UDamageShot> DamageType;

	UWorld* World;

	FHitResult Hit;

public:
	virtual void AttachWeapon(AMultiplayerProjectCharacter* TargetCharacter) override;

	virtual void GainAmmo() override;
protected:
	virtual void Fire() override;

	UFUNCTION(Server, Reliable)
	void ServerPerformLineTrace(FVector Direction);

	virtual void ActivateWeapon() override;

	virtual void Reload() override;



};
