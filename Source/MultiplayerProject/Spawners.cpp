// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawners.h"
#include "TP_WeaponComponent.h"
#include "TP_PickUpComponent.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASpawners::ASpawners()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
}

// Called when the game starts or when spawned
void ASpawners::BeginPlay()
{
	Super::BeginPlay();
    SpawnWeapons();

}

void ASpawners::SpawnWeapons()
{
    ServerSpawnWeapons();
}

void ASpawners::ServerSpawnWeapons_Implementation()
{
    World = GetWorld();
    if (IsValid(World))
    {
        WeaponClass = GetRandomWeaponClass();
        if (WeaponClass != nullptr)
        {
            FActorSpawnParameters SpawnParams;
            AActor* Weapon = World->SpawnActor<AActor>(WeaponClass, GetActorLocation(), GetActorRotation(), SpawnParams);
            if (Weapon)
            {
                UTP_PickUpComponent* Pickup = Weapon->FindComponentByClass<UTP_PickUpComponent>();
                if (Pickup)
                {
                    Pickup->GetSpawner(this);
                }
            }
        }
    }
}

TSubclassOf<AActor> ASpawners::GetRandomWeaponClass() const
{
    if (WeaponsToSpawn.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, WeaponsToSpawn.Num() - 1);
        return WeaponsToSpawn[RandomIndex];
    }
    return nullptr;
}

void ASpawners::OnRep_WeaponClass()
{
    if (!HasAuthority() && WeaponClass != nullptr)
    {
        FActorSpawnParameters SpawnParams;
        AActor* Weapon = GetWorld()->SpawnActor<AActor>(WeaponClass, GetActorLocation(), GetActorRotation(), SpawnParams);
        if (Weapon)
        {
            UTP_PickUpComponent* Pickup = Weapon->FindComponentByClass<UTP_PickUpComponent>();
            if (Pickup)
            {
                Pickup->GetSpawner(this);
            }
        }
    }
}
void ASpawners::Collected()
{
	World->GetTimerManager().SetTimer(Timer, this, &ASpawners::SpawnWeapons, 60, false);
}

// Called every frame
void ASpawners::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawners::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ASpawners, WeaponClass);
}

