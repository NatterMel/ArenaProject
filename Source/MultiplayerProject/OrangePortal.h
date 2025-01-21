// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PortalPog.h"
#include "OrangePortal.generated.h"

/**
 * 
 */
UCLASS(config = Game)
class MULTIPLAYERPROJECT_API AOrangePortal : public APortalPog
{
	GENERATED_BODY()
	
		AOrangePortal();

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent* CollisionOrange;

protected:
	virtual void BeginPlay() override;

	USceneCaptureComponent2D* MyCamera;

	bool bShouldExit = true;

public:

	UFUNCTION()
		void OnCollisionBeginOverlap(class UPrimitiveComponent* OverlappedComponent,
			class AActor* OtherActor,
			class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);
	void Bruh();
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UCapsuleComponent* Capsule();
	bool GetExit();

	void SetBool() { bShouldExit = false; }
};
