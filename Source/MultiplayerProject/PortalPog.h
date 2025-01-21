// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "PortalPog.generated.h"

UCLASS()
class MULTIPLAYERPROJECT_API APortalPog : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	USceneCaptureComponent2D* SceneCapture;

	UPROPERTY(VisibleAnywhere, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PlaneMesh;

	UPROPERTY(VisibleAnywhere, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	USceneComponent* FakePortal;

	UPROPERTY(VisibleAnywhere, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Top;

	UPROPERTY(VisibleAnywhere, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Bottom;

	UPROPERTY(VisibleAnywhere, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Left;

	UPROPERTY(VisibleAnywhere, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Right;

	UPROPERTY(EditAnywhere, Category = "Collision")
	TEnumAsByte<ECollisionChannel> Line = ECC_Pawn;
	
public:	
	// Sets default values for this actor's properties
	APortalPog();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void LineTrace(USceneComponent* Point);

	FHitResult HitTop;

	FHitResult HitBottom;

	FHitResult HitLeft;

	FHitResult HitRight;

	UCameraComponent* Camera;

public:
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	FVector Position();

	FVector Relative();

	USceneCaptureComponent2D* GetCamera();
private:
	float Counter;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
