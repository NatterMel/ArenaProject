// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalGunProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MultiplayerProjectCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "PortalPog.h"
#include "Engine/Engine.h"

// Sets default values
APortalGunProjectile::APortalGunProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    // Initialize the Collision Component first
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(5.0f);
    CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
    CollisionComp->OnComponentHit.AddDynamic(this, &APortalGunProjectile::OnHit);
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

void APortalGunProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
        UWorld* World = GetWorld();
        if (!Cast<AMultiplayerProjectCharacter>(OtherActor))
        {
            MyPortal = GetWorld()->SpawnActor<APortalPog>(ActorToSpawn, Hit.Location, Hit.ImpactNormal.Rotation());
            Destroy();
        }

}


