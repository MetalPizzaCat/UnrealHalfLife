// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponSystemCPP/WeaponFireComponents/BaseFireComponent.h"
#include "WeaponSystemCPP/WeaponBase.h"
#include "MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class UHL_API UMeleeAttack : public UBaseFireComponent
{
	GENERATED_BODY()
	
public:
	virtual void Fire(EFireType type, FVector location, FRotator rotation, AWeaponBase* weapon)override;
};
