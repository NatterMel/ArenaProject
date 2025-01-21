// Fill out your copyright notice in the Description page of Project Settings.


#include "OrangePortal.h"
#include "EngineUtils.h"
#include "MultiplayerProjectCharacter.h"
#include "Math/UnrealMathUtility.h"
#include "Components/CapsuleComponent.h"
#include "BluePortal.h"
#include <cmath>
#include "Components/PrimitiveComponent.h"
#include <Kismet/GameplayStatics.h>
#include "RocketProjectile.h"

AOrangePortal::AOrangePortal()
{
    CollisionOrange = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
    CollisionOrange->SetupAttachment(RootComponent);
    CollisionOrange->SetCapsuleSize(42, 120);
    CollisionOrange->SetRelativeLocation(FVector(-20, 0, 0), false);
}

void AOrangePortal::BeginPlay()
{
    Super::BeginPlay();
    CollisionOrange->OnComponentBeginOverlap.AddDynamic(this, &AOrangePortal::OnCollisionBeginOverlap);
    CollisionOrange->OnComponentEndOverlap.AddDynamic(this, &AOrangePortal::OnOverlapEnd);
    for (TActorIterator<AOrangePortal> ActorOrange(GetWorld()); ActorOrange; ++ActorOrange)
    {
        if (*ActorOrange != this)
        {
            AOrangePortal* CurrentActor = *ActorOrange;
            CurrentActor->Destroy();
        }

    }
}

void AOrangePortal::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if(bShouldExit == true)
    {
        AMultiplayerProjectCharacter* Character = Cast<AMultiplayerProjectCharacter>(OtherActor);
        ARocketProjectile* Projectile = Cast<ARocketProjectile>(OtherActor);


        if (OtherActor == Character || OtherActor == Projectile)
        {
            TArray<AActor*> FoundActors;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABluePortal::StaticClass(), FoundActors);
            for (AActor* Actor : FoundActors)
            {
                 ABluePortal* Portal = Cast<ABluePortal>(Actor);
                Portal->SetBool();
                OtherActor->SetActorLocation(Actor->GetActorLocation());
                if (OtherActor == Character)
                {
                    FVector Velocity = OtherActor->GetVelocity();
                    FVector Positive = FVector(FMath::Abs(Velocity.X), FMath::Abs(Velocity.Y), FMath::Abs(Velocity.Z));
                    FVector NewVelocity = Positive * Actor->GetActorForwardVector();
                    Character->LaunchCharacter(NewVelocity, true, true);
                }
            }
        }
    }
    
}

void AOrangePortal::Bruh()
{
    for (TActorIterator<ABluePortal> ActorBlue(GetWorld()); ActorBlue; ++ActorBlue)
    {
        ABluePortal* CurrentActor = *ActorBlue;
        if (CurrentActor != nullptr)
        {
            APortalPog* BlueParent = Cast<APortalPog>(CurrentActor);
            APortalPog* MyParent = Cast<APortalPog>(this);
            if (BlueParent && MyParent)
            {
                MyCamera = MyParent->GetCamera();
                if (MyCamera)
                {

                    FVector Scene = BlueParent->Position();
                    FVector Relative = BlueParent->Relative();
                    if (Scene != FVector::ZeroVector)
                    {
                        FVector Distance = Scene - BlueParent->GetActorLocation();
                        float Length = Distance.Size();
                        if (Length < 1)
                        {
                            Length = 1;
                        }
                        float divide = 1000 / Length;
                        float atans = FMath::RadiansToDegrees(atan(divide));
                        float ClampedValue = FMath::Clamp(atans, 5, 180);
                        MyCamera->FOVAngle = ClampedValue;
                        FRotator PortalRotation = BlueParent->GetActorRotation();
                        FVector WorldXVector = FVector::RightVector;
                        MyCamera->SetRelativeLocation(PortalRotation.RotateVector(Distance));


                        // Calculate rotation based on the difference between camera and parent positions
                        FVector CameraLocation = MyCamera->GetComponentLocation();
                        FVector ParentLocation = this->GetActorLocation();
                        FVector RelativeLocation = ParentLocation - CameraLocation;
                        FRotator Rot = FRotationMatrix::MakeFromX(RelativeLocation).Rotator();

                        // Set the world rotation of OCamera
                        MyCamera->SetWorldRotation(Rot);
                    }
                }
            }
        }
    }

}

void AOrangePortal::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AMultiplayerProjectCharacter* Character = Cast<AMultiplayerProjectCharacter>(OtherActor);
    ARocketProjectile* Projectile = Cast<ARocketProjectile>(OtherActor);

    if (OtherActor == Character || OtherActor == Projectile)
    {
        bShouldExit = true;
    }
}

UCapsuleComponent* AOrangePortal::Capsule()
{
    return CollisionOrange;
}

bool AOrangePortal::GetExit()
{
    return bShouldExit;
}