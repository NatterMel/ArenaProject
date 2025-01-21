// Fill out your copyright notice in the Description page of Project Settings.


#include "BluePortal.h"
#include "EngineUtils.h"
#include "MultiplayerProjectCharacter.h"
#include "Math/UnrealMathUtility.h"
#include "Components/CapsuleComponent.h"
#include "OrangePortal.h"
#include <cmath>
#include "Components/PrimitiveComponent.h"
#include <Kismet/GameplayStatics.h>
#include "RocketProjectile.h"

ABluePortal::ABluePortal()
{
	CollisionBlue = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
	CollisionBlue->SetupAttachment(RootComponent);
}

void ABluePortal::BeginPlay()
{

    Super::BeginPlay();
    CollisionBlue->OnComponentBeginOverlap.AddDynamic(this, &ABluePortal::OnCollisionBeginOverlap);
    CollisionBlue->OnComponentEndOverlap.AddDynamic(this, &ABluePortal::OnOverlapEnd);

    for (TActorIterator<ABluePortal> ActorBlue(GetWorld()); ActorBlue; ++ActorBlue)
    {
        if (*ActorBlue != this)
        {
            ABluePortal* CurrentActor = *ActorBlue;
            CurrentActor->Destroy();
        }

    }
}

void ABluePortal::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{    
    if(bShouldExit == true)
    {
        AMultiplayerProjectCharacter* Character = Cast<AMultiplayerProjectCharacter>(OtherActor);
        ARocketProjectile* Projectile = Cast<ARocketProjectile>(OtherActor);

        if (OtherActor == Character || OtherActor == Projectile)
        {
            TArray<AActor*> FoundActors;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrangePortal::StaticClass(), FoundActors);
            for (AActor* Actor : FoundActors)
            {
                AOrangePortal* Portal = Cast<AOrangePortal>(Actor);
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

void ABluePortal::Bruh()
{
    for (TActorIterator<AOrangePortal> ActorOrange(GetWorld()); ActorOrange; ++ActorOrange)
    {
        AOrangePortal* CurrentActor = *ActorOrange;

        if (CurrentActor != nullptr)
        {
            APortalPog* OranParent = Cast<APortalPog>(CurrentActor);
            APortalPog* MyParent = Cast<APortalPog>(this);
            if (OranParent && MyParent)
            {
                MyCamera = MyParent->GetCamera();
                if (MyCamera)
                {

                    FVector Scene = MyParent->Position();
                    FVector Relative = MyParent->Relative();
                    if (Scene != FVector::ZeroVector)
                    {
                        FVector Distance = Scene - OranParent->GetActorLocation();
                        float Length = Distance.Size();
                        if (Length < 1)
                        {
                            Length = 1;
                        }
                        float divide = 1000 / Length;
                        float atans = FMath::RadiansToDegrees(atan(divide));
                        float ClampedValue = FMath::Clamp(atans, 5, 180);
                        MyCamera->FOVAngle = ClampedValue;
                        FRotator PortalRotation = OranParent->GetActorRotation();
                        // Set the relative location of OCamera to match the Scene's relative location
                        if (!(this->GetActorForwardVector() + OranParent->GetActorForwardVector() == FVector::ZeroVector))
                        {
                            MyCamera->SetRelativeLocation(PortalRotation.RotateVector(Distance) * -1);
                        }
                        else
                        {
                            UE_LOG(LogTemp, Log, TEXT("Your log message here"));
                            MyCamera->SetRelativeLocation(PortalRotation.RotateVector(Distance));
                        }
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

void ABluePortal::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AMultiplayerProjectCharacter* Character = Cast<AMultiplayerProjectCharacter>(OtherActor);
    ARocketProjectile* Projectile = Cast<ARocketProjectile>(OtherActor);

    if (OtherActor == Character || OtherActor == Projectile)
    {
        bShouldExit = true;
    }
    
}

UCapsuleComponent* ABluePortal::Capsule()
{
    return CollisionBlue;
}
bool ABluePortal::GetExit()
{
    return bShouldExit;
}