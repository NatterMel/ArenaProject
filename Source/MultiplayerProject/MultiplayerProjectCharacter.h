// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TP_WeaponComponent.h"
#include "MultiplayerProjectCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class AMultiplayerProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* GunMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeAction2;
public:
	AMultiplayerProjectCharacter();

protected:
	virtual void BeginPlay();

public:
		
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle() const { return bHasRifle; }

	void SetCanChange(bool Can) { CanChange = Can; }

	UFUNCTION(NetMulticast, Reliable)
	void AddAmmo(UTP_WeaponComponent* Weapon);

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	TArray<UTP_WeaponComponent*> MyWeapons;
	int CurrentWeapon;

	UFUNCTION(NetMulticast, Reliable)
	void ChangeIndex(int Direction);

	void Change1(const FInputActionValue& Value);
	void Change2(const FInputActionValue& Value);
	UPROPERTY(ReplicatedUsing = OnRep_SetMaterial)
	UMaterialInterface* Material;

	UPROPERTY(VisibleInstanceOnly ,ReplicatedUsing = OnRep_GunSetMaterial)
	UMaterialInterface* GunMaterial;

	bool CanChange = true;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

		/** The player's maximum health. This is the highest value of their health can be.
This value is a value of the player's health, which starts at when spawned.*/
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth;
	/** The player's current health. When reduced to 0, they are considered dead.*/
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth;
	/** RepNotify for changes made to current health.*/
	UFUNCTION()
	void OnRep_CurrentHealth();

	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float CurrentAmmo;
	float ExtraAmmo;

	/** Response to health being updated. Called on the server immediately after
modification, and on clients in response to a RepNotify*/
	void OnHealthUpdate();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUD", Meta = (BlueprintProtected = true))
		TSubclassOf<class UUserWidget> HUDHealth;
	/** The instance of the HUD */
	UPROPERTY()
		class UUserWidget* CurrentWidget;
	/** The instance where spawn */

		bool Portal = false;
public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	USkeletalMeshComponent* GetGunMesh() const { return GunMesh; }

	UFUNCTION()
	void OnRep_SetMaterial();

	UFUNCTION()
	void OnRep_GunSetMaterial();

	void MaterialChange();

	void GunMaterialChange();

	void SetMat(UMaterialInterface* Matrial);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void Change3D(UTP_WeaponComponent* Gun);

	void ChangeGun(UTP_WeaponComponent* Gun);

	void HasPortal() { Portal = true; }

	/** Getter for Max Health.*/
	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	/** Getter for Current Health.*/
	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	/** Setter for Current Health. Clamps the value between 0 and MaxHealth and calls
	OnHealthUpdate. Should only be called on the server.*/
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetCurrentHealth(float healthValue);
	/** Event for taking damage. Overridden from APawn.*/
	UFUNCTION(BlueprintCallable, Category = "Health")
	float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category = "Ammo")
		void SetCurrentAmmo(float ammoValue);

	UFUNCTION(BlueprintCallable, Category = "Ammo")
		void SetExtratAmmo(float ammoextraValue);

	UFUNCTION(BlueprintPure, Category = "Ammo")
		FORCEINLINE float GetCurrentAmmo() const { return CurrentAmmo; }

	UFUNCTION(BlueprintPure, Category = "Ammo")
		FORCEINLINE float GetExtraAmmo() const { return ExtraAmmo; }

		void Die();
};

