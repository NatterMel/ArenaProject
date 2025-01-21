// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

ARocketProjectile::ARocketProjectile()
{
    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
    DamageType = UDamageType::StaticClass();

    // Initialize the Collision Component first
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(5.0f);
    CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
    CollisionComp->OnComponentHit.AddDynamic(this, &ARocketProjectile::OnHit);
    RootComponent = CollisionComp;

    // Initialize the Sphere Component and attach it to the Collision Component
    Sphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
    Sphere->SetupAttachment(CollisionComp);
    Sphere->SetVisibility(true);

    // Initialize the Projectile Movement Component
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = 3000.f;
    ProjectileMovement->MaxSpeed = 3000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->ProjectileGravityScale = 0.0f;

    bReplicates = true;
}

void ARocketProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    float MinimumDamage = 0.0f;
    float DamageFalloff = 1.0f;
    TArray<AActor*> IgnoredActors;
    UGameplayStatics::ApplyRadialDamageWithFalloff(this, Damage, MinimumDamage, GetActorLocation(), 50.0f, DamageRadius, DamageFalloff, nullptr, IgnoredActors, this, GetInstigatorController(), ECC_Visibility);
    Destroy();
}

void ARocketProjectile::Destroyed()
{
    FVector spawnLocation = GetActorLocation();
    UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, spawnLocation,
        FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
}




