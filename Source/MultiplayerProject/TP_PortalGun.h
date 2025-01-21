// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TP_WeaponComponent.h"
#include "TP_PortalGun.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MULTIPLAYERPROJECT_API UTP_PortalGun : public UTP_WeaponComponent
{
	GENERATED_BODY()

public:
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class APortalGunProjectile> OrangeClass;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class APortalGunProjectile> BlueClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SecondaryAction;

	UFUNCTION(Server, Reliable)
	void SpawnProjectileBlue(FVector Direction);

	UFUNCTION(Server, Reliable)
	void SpawnProjectileOrange(FVector Direction);

	virtual void AttachWeapon(AMultiplayerProjectCharacter* TargetCharacter) override;

	UWorld* World;

protected:
	virtual void Fire() override;

	virtual void SecondFire() override;

	virtual void ActivateWeapon() override;
};
