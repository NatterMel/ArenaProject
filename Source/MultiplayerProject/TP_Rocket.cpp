// Fill out your copyright notice in the Description page of Project Settings.


#include "TP_Rocket.h"
#include "MultiplayerProjectCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "RocketProjectile.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Math/UnrealMathUtility.h"

void UTP_Rocket::AttachWeapon(AMultiplayerProjectCharacter* TargetCharacter)
{
    Character = TargetCharacter;
    World = GetWorld();
    ReserveAmmo = StartAmmo * 2;
    Ammo = StartAmmo;
    if (Character == nullptr)
    {
        return;
    }
    // Attach the weapon to the First Person Character
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
    AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

    Character->SetCurrentAmmo(Ammo);
    Character->SetExtratAmmo(ReserveAmmo);

    // Switch bHasRifle so the animation blueprint can switch to another animation set
    Character->SetHasRifle(true);


    AActor* OwnerActor = GetOwner();
    OwnerActor->SetOwner(Character);
    SetOnlyOwnerSee(true);
    ActivateWeapon();
}


void UTP_Rocket::GainAmmo()
{
    ReserveAmmo += StartAmmo;
    Character->SetCurrentAmmo(Ammo);
    Character->SetExtratAmmo(ReserveAmmo);
}

void UTP_Rocket::Fire()
{
    if (!bIsShooting && Ammo > 0)
    {
        FVector Direction = Character->GetFirstPersonCameraComponent()->GetForwardVector() + FVector(0, -0.009, -0.05);
        bIsShooting = true;
        SpawnProjectile(Direction);
        World->GetTimerManager().SetTimer(FiringTimer, this, &UTP_Rocket::StopFire, FireRate, false);
         --Ammo;
         Character->SetCurrentAmmo(Ammo);
         Character->SetExtratAmmo(ReserveAmmo);
        if (Ammo <= 0)
        {
            Reload();
        }
    }
}

void UTP_Rocket::ActivateWeapon()
{
    TArray<AActor*> temp;
    Character->GetAttachedActors(temp);
    Character->SetCurrentAmmo(Ammo);
    Character->SetExtratAmmo(ReserveAmmo);

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
            EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_Rocket::Fire);
            EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &UTP_Rocket::Reload);
        }
    }
}

void UTP_Rocket::Reload()
{
    if (ReserveAmmo > 0)
    {
        Character->SetCanChange(false);
        float AmmoNeeded = StartAmmo - Ammo;
        float AmmoReload = FMath::Min(AmmoNeeded, StartAmmo);
        if (ReserveAmmo <= AmmoReload)
        {
            AmmoReload = ReserveAmmo;
        }
        ReserveAmmo -= AmmoReload;
        Ammo += AmmoReload;
        bIsShooting = true;
        Character->SetCurrentAmmo(Ammo);
        Character->SetExtratAmmo(ReserveAmmo);
        World->GetTimerManager().SetTimer(FiringTimer, this, &UTP_Rocket::StopFire, 2, false);
    }
}

void UTP_Rocket::SpawnProjectile_Implementation(FVector Direction)
{
    FVector SpawnLocation = Character->GetFirstPersonCameraComponent()->GetComponentLocation() + (Character->GetActorRotation().Vector() * 100.0f);
    FRotator SpawnRotation = Character->GetActorRotation();

    FActorSpawnParameters spawnParameters;
    spawnParameters.Instigator = Character->GetInstigator();
    spawnParameters.Owner = Character;
    spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
          ARocketProjectile* SpawnedProjectile = World->SpawnActor<ARocketProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, spawnParameters);


           if (SpawnedProjectile)
             {
        UProjectileMovementComponent* Move = SpawnedProjectile->GetProjectileMovement();
        Move->Velocity = Direction * 3000;
        SpawnedProjectile->SetDamage(Damage, DamageRadius); 
    }

}