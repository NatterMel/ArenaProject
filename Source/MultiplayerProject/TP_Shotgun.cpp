// Fill out your copyright notice in the Description page of Project Settings.


#include "TP_Shotgun.h"
#include "MultiplayerProjectCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DamageShot.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Camera/CameraComponent.h"
#include "Math/UnrealMathUtility.h"

void UTP_Shotgun::AttachWeapon(AMultiplayerProjectCharacter* TargetCharacter)
{
    DamageType = UDamageType::StaticClass();
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

void UTP_Shotgun::GainAmmo()
{
    ReserveAmmo += StartAmmo;
    Character->SetCurrentAmmo(Ammo);
    Character->SetExtratAmmo(ReserveAmmo);
}

void UTP_Shotgun::Fire()
{
    if (!bIsShooting && Ammo > 0)
    {
        FVector Direction = Character->GetFirstPersonCameraComponent()->GetForwardVector() + FVector(0, -0.009, -0.05);
        bIsShooting = true;
        ServerPerformLineTrace(Direction);
        World->GetTimerManager().SetTimer(FiringTimer, this, &UTP_Shotgun::StopFire, FireRate, false);
        --Ammo;
        Character->SetCurrentAmmo(Ammo);
        Character->SetExtratAmmo(ReserveAmmo);
        if (Ammo <= 0)
        {
            Reload();
        }
    }
}

void UTP_Shotgun::ActivateWeapon()
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
            EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_Shotgun::Fire);
            EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &UTP_Shotgun::Reload);
        }
    }
}

void UTP_Shotgun::Reload()
{
    UE_LOG(LogTemp, Warning, TEXT("Failed to spawn actor."));
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
        World->GetTimerManager().SetTimer(FiringTimer, this, &UTP_Shotgun::StopFire, 2, false);
    }
}

void UTP_Shotgun::ServerPerformLineTrace_Implementation(FVector Direction)
{
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Character);
    for (size_t i = 0; i < 6; i++)
    {
        FVector Shot = Direction * 1000.0f;
        FVector TraceStart = Character->GetFirstPersonCameraComponent()->GetComponentLocation();
        FVector TraceEnd = TraceStart + Shot;
        TraceEnd.Z = TraceEnd.Z + FMath::RandRange(Spread * -1, Spread);
        TraceEnd.Y = TraceEnd.Y + FMath::RandRange(Spread * -1, Spread);
        TraceEnd.X = TraceEnd.X + FMath::RandRange(Spread * -1, Spread);

        GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
        DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Hit.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, 0, 10.0f);

        if (Cast<AMultiplayerProjectCharacter>(Hit.GetActor()))
        {
            AActor* Actor = Hit.GetActor();
            UGameplayStatics::ApplyDamage(Actor, Damage, GetOwner()->GetInstigatorController(), Character, DamageType);
        }
    }

}
