// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponSystemCPP/WeaponFireComponents/BaseFireComponent.h"
#include "Engine/UserDefinedStruct.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "WeaponSystemCPP/Projectiles/BaseProjectile.h"
#include "WeaponData.generated.h"

UENUM(BlueprintType)
enum class EReloadType :uint8
{
	RTE_Clip UMETA(DisplayName = "Whole Clip"),
	RTE_OneBullet UMETA(DisplayName = "One bullet")
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct UHL_API FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

public:

	/*This name MUST be the same as row name*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString DisplayName = "null";

	/*For displaying it in ui*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int CategoryId;

	/*If weapon is melee there will be additional raycast to set proper animation*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool Melee = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool OnlyViewModel = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool OnlyWorldModel = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimBlueprint* AnimBlueprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool SecondaryFireUsesPrimaryClip = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PrimaryFire)
		EReloadType PrimaryReloadType = EReloadType::RTE_Clip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecondaryFire)
		EReloadType SecondaryReloadType = EReloadType::RTE_Clip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMesh* WorldMesh;

	/*Name of the row in animation data table*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString AnimationDataName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PrimaryFire)
		float PrimarySpread = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecondaryFire)
		float SecondarySpread = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PrimaryFire)
		float PrimaryDamage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecondaryFire)
		float SecondaryDamage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PrimaryFire)
		FString PrimaryAmmoType = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecondaryFire)
		FString SecondaryAmmoType = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PrimaryFire)
		TSubclassOf<UDamageType> PrimaryDamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecondaryFire)
		TSubclassOf<UDamageType> SecondaryDamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PrimaryFire)
		float PrimaryTimeBetweenShots = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecondaryFire)
		float SecondaryTimeBetweenShots = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PrimaryFire)
		float PrimarySpreadIncrement = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecondaryFire)
		int SecondarySpreadIncrement = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PrimaryFire)
		/*Only if primary component uses bullets*/
		int PrimaryBulletsPerShot = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecondaryFire)
		/*Only if Secondary component uses bullets*/
		int SecondaryBulletsPerShot = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PrimaryFire)
		/*Only if primary component uses bullets*/
		int PrimaryAmmoPerClip = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecondaryFire)
		/*Only if Secondary component uses bullets*/
		int SecondaryAmmoPerClip = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PrimaryFire)
		float PrimaryRange = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecondaryFire)
		float SecondaryRange = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PrimaryFire)
		float PrimaryTimeToReload = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecondaryFire)
		float SecondaryTimeToReload = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PrimaryFire)
		class 	UParticleSystem* PrimaryFireMuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecondaryFire)
		class 	UParticleSystem* SecondaryFireMuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PrimaryFire)
		TSubclassOf<UBaseFireComponent> PrimaryFireComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = PrimaryFire)
		TSubclassOf<ABaseProjectile> PrimaryProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecondaryFire)
		TSubclassOf<UBaseFireComponent> SecondaryFireComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = SecondaryFire)
		TSubclassOf<ABaseProjectile> SecondaryProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PrimaryFire)
		USoundBase* PrimaryFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SecondaryFire)
		USoundBase* SecondarFireSound;





	/*Return this struct not as poiner
	*/
	FWeaponData Get()
	{
		FWeaponData res;
		res.DisplayName = this->DisplayName;
		res.PrimarySpread = this->PrimarySpread;
		res.SecondarySpread = this->SecondarySpread;
		res.PrimaryDamage = this->PrimaryDamage;
		res.SecondaryDamage = this->SecondaryDamage;
		res.PrimaryTimeBetweenShots = this->PrimaryTimeBetweenShots;
		res.SecondaryTimeBetweenShots = this->SecondaryTimeBetweenShots;
		res.PrimarySpreadIncrement = this->PrimarySpreadIncrement;
		res.SecondarySpreadIncrement = this->SecondarySpreadIncrement;
		res.PrimaryBulletsPerShot = this->PrimaryBulletsPerShot;
		res.SecondaryBulletsPerShot = this->SecondaryBulletsPerShot;
		res.PrimaryRange = this->PrimaryRange;
		res.SecondaryRange = this->SecondaryRange;
		res.PrimaryFireMuzzleFlash = this->PrimaryFireMuzzleFlash;
		res.SecondaryFireMuzzleFlash = this->SecondaryFireMuzzleFlash;
		res.PrimaryFireComponent = this->PrimaryFireComponent;
		res.SecondaryFireComponent = this->SecondaryFireComponent;
		res.PrimaryProjectileClass = this->PrimaryProjectileClass;
		res.SecondaryProjectileClass = this->SecondaryProjectileClass;
		res.PrimaryFireSound = this->PrimaryFireSound;
		res.SecondarFireSound = this->SecondarFireSound;
		res.Mesh = this->Mesh;
		res.OnlyWorldModel = this->OnlyWorldModel;
		res.OnlyViewModel = this->OnlyViewModel;
		res.Melee = this->Melee;
		res.WorldMesh = this->WorldMesh;
		res.PrimaryAmmoPerClip = this->PrimaryAmmoPerClip;
		res.SecondaryAmmoPerClip = this->SecondaryAmmoPerClip;
		res.SecondaryFireUsesPrimaryClip = this->SecondaryFireUsesPrimaryClip;
		res.PrimaryTimeToReload = this->PrimaryTimeToReload;
		res.SecondaryTimeToReload = this->SecondaryTimeToReload;
		res.AnimationDataName = this->AnimationDataName;
		res.CategoryId = this->CategoryId;
		res.PrimaryReloadType = this->PrimaryReloadType;
		res.SecondaryReloadType = this->SecondaryReloadType;
		res.PrimaryDamageType = this->PrimaryDamageType;
		res.SecondaryDamageType = this->SecondaryDamageType;
		res.PrimaryAmmoType = this->PrimaryAmmoType;
		res.SecondaryAmmoType = this->SecondaryAmmoType;

		return res;
	}

	FWeaponData()
	{

	}
};
/*: public UUserDefinedStruct*/