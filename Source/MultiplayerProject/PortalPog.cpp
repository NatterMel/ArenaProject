// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalPog.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/World.h"
#include "MultiplayerProjectCharacter.h"
#include "Camera/CameraComponent.h"

// Sets default values
APortalPog::APortalPog()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneComponent"));
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh"));
	FakePortal = CreateDefaultSubobject<USceneComponent>(TEXT("Fake"));
	Top = CreateDefaultSubobject<USceneComponent>(TEXT("Top"));
	Bottom = CreateDefaultSubobject<USceneComponent>(TEXT("Bottom"));
	Left = CreateDefaultSubobject<USceneComponent>(TEXT("Left"));
	Right = CreateDefaultSubobject<USceneComponent>(TEXT("Right"));
	RootComponent = PlaneMesh;
	SceneCapture->SetupAttachment(RootComponent);
	FakePortal->SetupAttachment(RootComponent);
	Top->SetupAttachment(FakePortal);
	Bottom->SetupAttachment(FakePortal);
	Left->SetupAttachment(FakePortal);
	Right->SetupAttachment(FakePortal);

}

// Called when the game starts or when spawned
void APortalPog::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		// Get the pawn possessed by the player controller (usually the player character)
		AMultiplayerProjectCharacter* Char = Cast<AMultiplayerProjectCharacter>(PlayerController->GetPawn());
		Camera = Char->GetFirstPersonCameraComponent();
	}
	Counter = 0;
	LineTrace(Top);
	LineTrace(Bottom);
	LineTrace(Left);
	LineTrace(Right);
	
}

void APortalPog::LineTrace(USceneComponent* Point)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	FVector StartLocation = Point->GetComponentLocation();
	FVector EndLocation = StartLocation + Point->GetForwardVector() * -50;
	FHitResult HitResult;

	bool bHitSomething = World->LineTraceSingleByChannel(
		HitResult,         // Output: Information about what was hit
		StartLocation,     // Start location of the trace
		EndLocation,       // End location of the trace
		ECollisionChannel::ECC_Visibility);

	DrawDebugLine(
		World,
		StartLocation,
		EndLocation,
		FColor::Green,
		false,
		1.f,
		0.f,
		10.f);
	if (Counter == 6)
	{
		if (HitRight.GetActor() != HitLeft.GetActor() || HitBottom.GetActor() != HitTop.GetActor())
		{
			this->Destroy();
		}
	}
	if (Counter < 6)
	{

		if (Point == Top)
		{
			HitTop = HitResult;
			if (!bHitSomething)
			{
				RootComponent->SetWorldLocation((RootComponent->GetComponentLocation() + (Point->GetUpVector() * -40)));
				++Counter;
				LineTrace(Top);
			}
		}
		if (Point == Bottom)
		{
			HitBottom = HitResult;
			if (!bHitSomething)
			{
				RootComponent->SetWorldLocation((RootComponent->GetComponentLocation() + (Point->GetUpVector() * 40)));
				++Counter;
				LineTrace(Bottom);
			}
		}
		if (Point == Left)
		{
			HitLeft = HitResult;
			if (!bHitSomething)
			{
				RootComponent->SetWorldLocation((RootComponent->GetComponentLocation() + (Point->GetRightVector() * -40)));
				++Counter;
				LineTrace(Left);
			}
		}
		if (Point == Right)
		{
			HitRight = HitResult;
			if (!bHitSomething)
			{
				RootComponent->SetWorldLocation((RootComponent->GetComponentLocation() + (Point->GetRightVector() * 40)));
				++Counter;
				LineTrace(Right);
			}
		}

	}
	else
	{
		this->Destroy();
	}

}


FVector APortalPog::Position()
{
	FVector PlayerPos = Camera->GetComponentLocation();
	return PlayerPos;
}

FVector APortalPog::Relative()
{
	FVector PlayerRel = Camera->GetRelativeLocation();
	return PlayerRel;
}

USceneCaptureComponent2D* APortalPog::GetCamera()
{
	return SceneCapture;
}

void APortalPog::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


