// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerProjectCharacter.h"
#include "MultiplayerProjectGameMode.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "PortalSpawner.h"
#include "TP_WeaponComponent.h"


//////////////////////////////////////////////////////////////////////////
// AMultiplayerProjectCharacter

AMultiplayerProjectCharacter::AMultiplayerProjectCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	//GunMesh
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	GunMesh->SetupAttachment(GetMesh(), "hand_r");

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

	bReplicates = true;

	CurrentAmmo = 0.0f;
	ExtraAmmo = 0.0f;
}

void AMultiplayerProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDHealth);
	if (CurrentWidget != nullptr)
	{

		CurrentWidget->AddToViewport();
	}

}

//////////////////////////////////////////////////////////////////////////// Input

void AMultiplayerProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMultiplayerProjectCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMultiplayerProjectCharacter::Look);

		EnhancedInputComponent->BindAction(ChangeAction, ETriggerEvent::Triggered, this, &AMultiplayerProjectCharacter::Change1);
		EnhancedInputComponent->BindAction(ChangeAction2, ETriggerEvent::Triggered, this, &AMultiplayerProjectCharacter::Change2);
	}
}

void AMultiplayerProjectCharacter::OnRep_SetMaterial()
{
	MaterialChange();
}

void AMultiplayerProjectCharacter::OnRep_GunSetMaterial()
{
	GunMaterialChange();
}

void AMultiplayerProjectCharacter::MaterialChange()
{
	USkeletalMeshComponent* Meshel = GetMesh();
	Meshel->SetMaterial(0, Material);
}

void AMultiplayerProjectCharacter::GunMaterialChange()
{
	if (GunMesh && GunMaterial)
	{
		GunMesh->SetMaterial(0, GunMaterial);
	}
}

void AMultiplayerProjectCharacter::SetMat(UMaterialInterface* Matrial)
{
	Material = Matrial;
	if (GetLocalRole() == ROLE_Authority )
	{
		MaterialChange();
	}
}

void AMultiplayerProjectCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiplayerProjectCharacter, Material);
	DOREPLIFETIME(AMultiplayerProjectCharacter, GunMaterial);
	DOREPLIFETIME(AMultiplayerProjectCharacter, CurrentHealth);
}

void AMultiplayerProjectCharacter::Change3D_Implementation(UTP_WeaponComponent* Gun)
{
	GunMesh->SetSkeletalMesh(Cast<USkeletalMeshComponent>(Gun)->SkeletalMesh);

	if (GetLocalRole() == ROLE_Authority)
	{	GunMaterial = Gun->GetMaterial(0);
		GunMaterialChange();
	}
}

void AMultiplayerProjectCharacter::ChangeGun(UTP_WeaponComponent* Gun)
{
	UClass* GunClass = Gun->GetClass();
	if (!MyWeapons.ContainsByPredicate([GunClass](const UTP_WeaponComponent* Weapon) { return Weapon->GetClass() == GunClass; }))
	{
		MyWeapons.Add(Gun);
		CurrentWeapon = MyWeapons.Find(Gun);
	}
		Change3D(Gun);
}


void AMultiplayerProjectCharacter::AddAmmo_Implementation(UTP_WeaponComponent* Weapon)
{
	if (!MyWeapons.IsEmpty())
	{
		for (UTP_WeaponComponent* Available : MyWeapons)
		{
			if (Available->GetClass() == Weapon->GetClass())
			{
				Available->GainAmmo();
				break;
			}
		}
	}
}

void AMultiplayerProjectCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AMultiplayerProjectCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMultiplayerProjectCharacter::ChangeIndex_Implementation(int Direction)
{
	if (!MyWeapons.IsEmpty())
	{
		if (Direction != 0)
		{
			CurrentWeapon = (CurrentWeapon + Direction + MyWeapons.Num()) % MyWeapons.Num();
		}
		if (MyWeapons.IsValidIndex(CurrentWeapon))
		{
			MyWeapons[CurrentWeapon]->ActivateWeapon();
		}
	}
}

void AMultiplayerProjectCharacter::Change1(const FInputActionValue& Value)
{	
		ChangeIndex(1);
}
void AMultiplayerProjectCharacter::Change2(const FInputActionValue& Value)
{
		ChangeIndex(-1);

}

void AMultiplayerProjectCharacter::OnHealthUpdate()
{
	//Client-specific functionality
	if (IsLocallyControlled())
	{
		if (CurrentHealth <= 0)
		{
			Die();
		}
	}
	//Server-specific functionality
	if (GetLocalRole() == ROLE_Authority)
	{
		if (CurrentHealth <= 0)
		{
			if (Portal)
			{
				TArray<AActor*> FoundActors;
				UGameplayStatics::GetAllActorsOfClass(GetWorld(), APortalSpawner::StaticClass(), FoundActors);
				for (AActor* Actor : FoundActors)
				{
					APortalSpawner* PortalSpawner = Cast<APortalSpawner>(Actor);
					if (PortalSpawner)
					{
						PortalSpawner->SpawnWeapons();
					}
				}
			}
			Die();
		}

	}
}

void AMultiplayerProjectCharacter::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void AMultiplayerProjectCharacter::SetCurrentHealth(float healthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
		OnHealthUpdate();
	}
}

float AMultiplayerProjectCharacter::TakeDamage(float DamageTaken, struct FDamageEvent const&
	DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageTaken, DamageEvent, EventInstigator, DamageCauser);
	float damageApplied = FMath::Clamp(DamageTaken, 0.0f, CurrentHealth);
	SetCurrentHealth(CurrentHealth - damageApplied);
	return damageApplied;
}

void AMultiplayerProjectCharacter::SetCurrentAmmo(float ammoValue)
{
	CurrentAmmo = ammoValue;
}

void AMultiplayerProjectCharacter::SetExtratAmmo(float ammoextraValue)
{
	ExtraAmmo = ammoextraValue;
}

void AMultiplayerProjectCharacter::Die()
{
	UMaterialInterface* CurrentMaterial = Material;

	AMultiplayerProjectGameMode* GM = Cast<AMultiplayerProjectGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->RespawnPlayer(Controller, CurrentMaterial);
	}	
	Destroy();
}

void AMultiplayerProjectCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}
