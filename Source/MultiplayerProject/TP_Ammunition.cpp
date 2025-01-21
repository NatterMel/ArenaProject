// Fill out your copyright notice in the Description page of Project Settings.


#include "TP_Ammunition.h"
#include "TP_WeaponComponent.h"
#include "MultiplayerProjectCharacter.h"

void UTP_Ammunition::CallAmmo(AMultiplayerProjectCharacter* TargetCharacter)
{
    if (TargetCharacter == nullptr)
    {
        return;
    }
    if (WeaponToAdd != nullptr)
    {
      TargetCharacter->AddAmmo(WeaponToAdd);
    }

    GetOwner()->Destroy();
}
