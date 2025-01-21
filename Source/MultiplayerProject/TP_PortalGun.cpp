// Fill out your copyright notice in the Description page of Project Settings.


#include "TP_PortalGun.h"
#include "MultiplayerProjectCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PortalGunProjectile.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Math/UnrealMathUtility.h"


void UTP_PortalGun::AttachWeapon(AMultiplayerProjectCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	World = GetWorld();

	// Check that the character is valid, and has no rifle yet
	if (Character == nullptr)
	{
		return;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	// switch bHasRifle so the animation blueprint can switch to another animation set
	Character->SetHasRifle(true);

	AActor* OwnerActor = GetOwner();
	OwnerActor->SetOwner(Character);
	Character->HasPortal();
	SetOnlyOwnerSee(true);
	ActivateWeapon();
}

void UTP_PortalGun::ActivateWeapon()
{
	TArray<AActor*> temp;
	Character->GetAttachedActors(temp);

	for (AActor* var : temp)
	{
		var->SetActorHiddenInGame(true);
	}
	GetOwner()->SetActorHiddenInGame(false);
	Character->ChangeGun(this);
	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->ClearActionBindings();
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_PortalGun::Fire);
			EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Triggered, this, &UTP_PortalGun::SecondFire);
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Some debug message!"));
		}
	}
}

void UTP_PortalGun::SecondFire()
{
	if (OrangeClass != nullptr)
	{
		FVector Direction = Character->GetFirstPersonCameraComponent()->GetForwardVector() + FVector(0, -0.009, -0.05);
		SpawnProjectileOrange(Direction);
	}
}

void UTP_PortalGun::Fire()
{
	if (BlueClass != nullptr)
	{
			FVector Direction = Character->GetFirstPersonCameraComponent()->GetForwardVector() + FVector(0, -0.009, -0.05);
			SpawnProjectileBlue(Direction);
	}
}

void UTP_PortalGun::SpawnProjectileBlue_Implementation(FVector Direction)
{
	FVector SpawnLocation = Character->GetFirstPersonCameraComponent()->GetComponentLocation() + (Character->GetActorRotation().Vector() * 100.0f);
	FRotator SpawnRotation = Character->GetActorRotation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = Character->GetInstigator();
	spawnParameters.Owner = Character;
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APortalGunProjectile* SpawnedProjectile = World->SpawnActor<APortalGunProjectile>(BlueClass, SpawnLocation, SpawnRotation, spawnParameters);

	if (SpawnedProjectile)
	{
		UProjectileMovementComponent* Move = SpawnedProjectile->GetProjectileMovement();
		Move->Velocity = Direction * 3000;
	}

}

void UTP_PortalGun::SpawnProjectileOrange_Implementation(FVector Direction)
{
	FVector SpawnLocation = Character->GetFirstPersonCameraComponent()->GetComponentLocation() + (Character->GetActorRotation().Vector() * 100.0f);
	FRotator SpawnRotation = Character->GetActorRotation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = Character->GetInstigator();
	spawnParameters.Owner = Character;
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APortalGunProjectile* SpawnedProjectile = World->SpawnActor<APortalGunProjectile>(OrangeClass, SpawnLocation, SpawnRotation, spawnParameters);

	if (SpawnedProjectile)
	{
		UProjectileMovementComponent* Move = SpawnedProjectile->GetProjectileMovement();
		Move->Velocity = Direction * 3000;
	}
}
