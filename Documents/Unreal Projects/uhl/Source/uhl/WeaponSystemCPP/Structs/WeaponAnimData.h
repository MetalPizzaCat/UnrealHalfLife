// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedStruct.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "Runtime/Engine/Classes/Animation/AnimSequence.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMesh.h"
#include "WeaponAnimData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct UHL_API FWeaponAnimData : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimSequenceBase* Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequenceBase* IdleEmpty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequenceBase* IdleToLow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequenceBase* LowIdle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequenceBase* LowToIdle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequenceBase* Draw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequenceBase* Holster;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequenceBase* DrawEmpty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequenceBase* HolsterEmpty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UAnimSequenceBase*> Fire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UAnimSequenceBase*> AltFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequenceBase* FireEmpty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequenceBase* AltFireEmpty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequenceBase* Reload;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimSequenceBase* AltReload;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Melee)
		TArray<UAnimSequenceBase*> Hit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Melee)
		TArray<UAnimSequenceBase*> Miss;
};
