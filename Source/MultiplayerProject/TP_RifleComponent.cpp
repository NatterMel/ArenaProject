// Fill out your copyright notice in the Description page of Project Settings.


#include "TP_RifleComponent.h"
#include "MultiplayerProjectCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DamageShot.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Camera/CameraComponent.h"
#include "CoreMinimal.h"  
#include "Math/UnrealMathUtility.h"

void UTP_RifleComponent::AttachWeapon(AMultiplayerProjectCharacter* TargetCharacter)
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

void UTP_RifleComponent::Fire()
{
    if (!bIsShooting && Ammo > 0)
    {
        FVector Direction = Character->GetFirstPersonCameraComponent()->GetForwardVector() + FVector(0, -0.009 ,-0.05);
        bIsShooting = true;
        ServerPerformLineTrace(Direction);
        World->GetTimerManager().SetTimer(FiringTimer, this, &UTP_RifleComponent::StopFire, FireRate, false);

        --Ammo;
        Character->SetCurrentAmmo(Ammo);
        Character->SetExtratAmmo(ReserveAmmo);
        if (Ammo <= 0)
        {
            Reload();
        }
    }
}

void UTP_RifleComponent::ActivateWeapon()
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
            EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_RifleComponent::Fire);
            EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &UTP_RifleComponent::Reload);
        }
    }
}

void UTP_RifleComponent::Reload()
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
        World->GetTimerManager().SetTimer(FiringTimer, this, &UTP_RifleComponent::StopFire, 2, false);
    }
}

void UTP_RifleComponent::GainAmmo()
{
    ReserveAmmo +=  StartAmmo;
    Character->SetCurrentAmmo(Ammo);
    Character->SetExtratAmmo(ReserveAmmo);
}

void UTP_RifleComponent::ServerPerformLineTrace_Implementation(FVector Direction)
{
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Character);
        FVector Shot = Direction * 5000.0f;
        FVector TraceStart = Character->GetFirstPersonCameraComponent()->GetComponentLocation();
        FVector TraceEnd = TraceStart + Shot;

        GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
        DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Hit.bBlockingHit ? FColor::Blue : FColor::Red, false, FireRate, 0, 10.0f);

        if (Cast<AMultiplayerProjectCharacter>(Hit.GetActor()))
        {
            AMultiplayerProjectCharacter* Actor = Cast<AMultiplayerProjectCharacter>(Hit.GetActor());
            USkeletalMeshComponent* MeshComp = Actor->GetMesh();
            if (MeshComp)
            {
                FName BoneName = Hit.BoneName; // Example BoneName, replace with actual variable
                FString BoneNameString = BoneName.ToString();
                FString healthMessage = FString::Printf(TEXT("You shot this %s."), *BoneNameString);
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, healthMessage);

                if (BoneName == "head")
                {
                    UGameplayStatics::ApplyDamage(Actor, 100, GetOwner()->GetInstigatorController(), Character, DamageType);
                }else
                {
                   UGameplayStatics::ApplyDamage(Actor, Damage, GetOwner()->GetInstigatorController(), Character, DamageType);
                }
            }
        }
}

