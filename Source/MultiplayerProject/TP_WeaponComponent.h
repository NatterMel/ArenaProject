// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "TP_WeaponComponent.generated.h"

class AMultiplayerProjectCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERPROJECT_API UTP_WeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReloadAction;

	/** Sets default values for this component's properties */
	UTP_WeaponComponent();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	virtual void AttachWeapon(AMultiplayerProjectCharacter* TargetCharacter) PURE_VIRTUAL(UTP_WeaponComponent::AttachWeapon,);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	virtual void Fire() PURE_VIRTUAL(UTP_WeaponComponent::Fire,);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void SecondFire() PURE_VIRTUAL(UTP_WeaponComponent::SecondFire,);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StopFire();

	virtual void GainAmmo() PURE_VIRTUAL(UTP_WeaponComponent::GainAmmo, );

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void ActivateWeapon() PURE_VIRTUAL(UTP_WeaponComponent::ActivateWeapon,);
protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void AmmoPickedUp(float Munition) { ReserveAmmo += Munition; }

	virtual void Reload() PURE_VIRTUAL(UTP_WeaponComponent::Reload,);


	/** The Character holding this weapon*/
	AMultiplayerProjectCharacter* Character;

	bool bIsShooting;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float Damage;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	float Ammo;


	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float StartAmmo;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	float ReserveAmmo;


private:

};
