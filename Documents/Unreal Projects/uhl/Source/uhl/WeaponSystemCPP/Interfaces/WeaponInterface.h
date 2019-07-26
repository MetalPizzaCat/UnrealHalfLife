// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UHL_API IWeaponInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactions")
		bool PickUpWeapon(UDataTable* WeaponDataTable,	UDataTable* AnimationDataTable,FName weaponName, int primaryAmountOfAmmo, int primaryAmmoInTheClip, int secondaryAmountOfAmmo, int secondaryAmmoInTheClip);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactions")
		bool PickUpAmmo(const FString& AmmoName, int32 Amount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Notify")
		void OwnerNotifyPrimaryReload();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Notify")
		void OwnerNotifySecondaryReload();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Notify")
		void OwnerNotifyEndPrimaryReload();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Notify")
		void OwnerNotifyEndSecondaryReload();

	
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};
