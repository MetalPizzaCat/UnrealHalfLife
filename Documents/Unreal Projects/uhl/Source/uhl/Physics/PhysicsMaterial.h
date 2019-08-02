// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PhysicsMaterial.generated.h"

/**
 * 
 */
UCLASS()
class UHL_API UPhysicsMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* HitSoftSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* HitHardSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* StepSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* BulletHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* BulletImpactParticleEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* ImpactParticleEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UMaterialInterface*> ShotDecalMaterial;
};
