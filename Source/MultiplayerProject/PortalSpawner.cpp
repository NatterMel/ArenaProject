// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalSpawner.h"
#include "TP_PortalGun.h"

// Sets default values
APortalSpawner::APortalSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APortalSpawner::BeginPlay()
{
	Super::BeginPlay();
	SpawnWeapons();
}

// Called every frame
void APortalSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APortalSpawner::SpawnWeapons()
{
	World = GetWorld();
	if (IsValid(World))
	{
		FActorSpawnParameters SpawnParams;
		AActor* Weapon = World->SpawnActor<AActor>(WeaponToSpawn, this->GetActorLocation(), this->GetActorRotation(), SpawnParams);
		UTP_PortalGun* Portal = Weapon->GetComponentByClass<UTP_PortalGun>();
	}
}

