// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawners.generated.h"

UCLASS()
class MULTIPLAYERPROJECT_API ASpawners : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawners();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawner")
	TArray<TSubclassOf<AActor>> WeaponsToSpawn;


	void SpawnWeapons();

	UFUNCTION(Server, Reliable)
	void ServerSpawnWeapons();

	UPROPERTY(ReplicatedUsing = OnRep_WeaponClass)
	TSubclassOf<AActor> WeaponClass;

	UFUNCTION()
	void OnRep_WeaponClass();

	TSubclassOf<AActor> GetRandomWeaponClass() const;

	UWorld* World;

	FTimerHandle Timer;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Collected();
};
