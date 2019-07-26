// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponSystemCPP/WeaponFireComponents/BaseFireComponent.h"
#include "WeaponSystemCPP/WeaponBase.h"
#include "ProjectileFireWeaponComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class UHL_API UProjectileFireWeaponComponent : public UBaseFireComponent
{
	GENERATED_BODY()
public:
	void Fire(EFireType type, FVector location, FRotator rotation, AWeaponBase* weapon)override;
};
