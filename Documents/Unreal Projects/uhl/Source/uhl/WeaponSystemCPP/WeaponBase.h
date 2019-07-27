// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponSystemCPP/Structs/WeaponData.h"
//#include "WeaponFireComponents/WeaponFireComponent.h"
#include "WeaponSystemCPP/Interfaces/WeaponInterface.h"
#include "WeaponSystemCPP/Structs/WeaponAnimData.h"
#include "WeaponBase.generated.h"

UCLASS()
class UHL_API AWeaponBase : public AActor, public IWeaponInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeaponBase();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
		FName Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = PrimaryFire)
		float PrimaryTimeSinceLastShot = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = PrimaryFire)
		bool PrimaryIsShooting = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = PrimaryFire)
		int PrimaryAmmoInTheClip = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = PrimaryFire)
		int PrimaryAmountOfAmmo = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = PrimaryFire)
		float PrimaryCurrentSpread = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
		bool CanBePickedUp = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
		bool IsReloading = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
		float TimeInReload = 0.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
		FWeaponData Data;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	FWeaponAnimData* AnimationData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
		UDataTable* WeaponDataTable;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
		UDataTable* AnimationDataTable;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = SecondaryFire)
		float SecondaryTimeSinceLastShot = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = SecondaryFire)
		bool SecondaryIsShooting = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = SecondaryFire)
		int SecondaryAmmoInTheClip = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = SecondaryFire)
		int SecondaryAmountOfAmmo = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = SecondaryFire)
		float SecondaryCurrentSpread = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	AActor* WeaponOwner;

	UFUNCTION(BlueprintCallable)
		bool PrimaryFire(FVector location, FRotator rotation);

	UFUNCTION(BlueprintCallable)
		bool SecondaryFire(FVector location, FRotator rotation);

	UFUNCTION(BlueprintCallable)
		bool Fire(EFireType type, FVector location, FRotator rotation);

	UFUNCTION(BlueprintCallable)
		void PrimaryReload();

	UFUNCTION(BlueprintCallable)
		void SecondaryReload();

	UFUNCTION(BlueprintCallable)
		void Reload(EFireType type);

	void EndPrimaryReload();


	void EndSecondaryReload();


	UFUNCTION(BlueprintCallable)
		void AbortPrimaryReload();

	UFUNCTION(BlueprintCallable)
		void AbortSecondaryReload();

	UFUNCTION(BlueprintCallable)
		void AbortReload(EFireType type);

	UFUNCTION(BlueprintPure)
		bool CanShoot(EFireType type);

	UFUNCTION(BlueprintCallable)
		bool LoadData(UDataTable* weaponDT, UDataTable* animationDT);

	UPROPERTY(EditAnywhere, Category = Mesh)
		class USkeletalMeshComponent* WeaponMesh;

	

	/*UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame,Category=PrimaryFire)
		class WeaponFireComponent* PrimaryFireComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = SecondaryFire)
		class WeaponFireComponent* SecondaryFireComponent;*/

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FWeaponData* data;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	


};
