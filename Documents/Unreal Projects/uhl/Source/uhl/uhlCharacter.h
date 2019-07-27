// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/CI_Interact.h"
#include "WeaponSystemCPP/WeaponBase.h"
#include "WeaponSystemCPP/Interfaces/WeaponInterface.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Physics/PhysicalActorBase.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsHandleComponent.h"
#include "uhlCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class AuhlCharacter : public ACharacter, public ICI_Interact, public IWeaponInterface
{
	GENERATED_BODY()

		/** Pawn mesh: 1st person view (arms; seen only by self) */
		UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* FP_Gun;

	

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* VR_Gun;

	

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* L_MotionController;


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = Health)
		float Health = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = Health)
		float MaxHealth = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = Health)
		bool Dead = false;

	UFUNCTION(BlueprintPure)
		float GetMaxHealth()const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		TArray<AWeaponBase*> Weapons;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		int  CurrentWeaponId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		int  CurrentWeaponCategoryId;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
		TMap<FString, int>Ammo;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
		bool UsingLadder = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = Suit)
		bool IsSuitOn = false;

	/*Basicly health of the suit*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = Suit)
		float SuitEnergy = 0;

	/*Basicly health of the suit*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = Suit)
		float MaxSuitEnergy = 100;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = Movement)
		float MaxSpeedSprinting = 900;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = Movement)
		float MaxSprintTime = 3.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = Movement)
		float SprintedTime = 0.f;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = Movement)
		bool bSprinting = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame, Category = Movement)
		bool bCanSprint = true;

	UFUNCTION(BlueprintPure)
		bool GetIsSprinting()const { return bSprinting; }

	UFUNCTION(BlueprintPure)
		float GetMaxSprintTime()const { return MaxSprintTime; }

	UFUNCTION(BlueprintPure)
		float GetMaxSpeedSprinting()const { return MaxSpeedSprinting; }

	UFUNCTION(BlueprintPure)
		float GetMaxSuitEnergy()const;

	UFUNCTION(BlueprintPure)
		bool GetIsSuitOn()const { return IsSuitOn; }

	UFUNCTION(BlueprintPure)
		AWeaponBase* GetCurrentWeapon();

	UFUNCTION(BlueprintPure)
		int GetCategoriesCount()const;

	UFUNCTION(BlueprintPure)
		TArray<AWeaponBase*> GetAllWeaponOfCategory(int CategoryId);

	UFUNCTION(BlueprintCallable)
		void StartUsingLadder();

	UFUNCTION(BlueprintCallable)
		void StopUsingLadder();

	UFUNCTION(BlueprintCallable)
		void StartSprinting();

	UFUNCTION(BlueprintCallable)
		void StopSprinting();

	UFUNCTION(BlueprintImplementableEvent)
		void WeaponChageAnimNote(int prev, int next);

	UFUNCTION(BlueprintImplementableEvent)
		void WeaponEndPrimaryReloadAnimNote();

	/*Due to bug in the UE4 you unable to override Tick event*/
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
		void AnimUpdateTick(float DeltaTime);

public:
	AuhlCharacter();


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool PickUpWeapon(UDataTable* WeaponDataTable, UDataTable* AnimationDataTable, FName weaponName, int primaryAmountOfAmmo, int primaryAmmoInTheClip, int secondaryAmountOfAmmo, int secondaryAmmoInTheClip);

	bool PickUpWeapon_Implementation(UDataTable* WeaponDataTable, UDataTable* AnimationDataTable, FName weaponName, int primaryAmountOfAmmo, int primaryAmmoInTheClip, int secondaryAmountOfAmmo, int secondaryAmmoInTheClip);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool PickUpAmmo(const FString& AmmoName, int32 Amount);

	bool PickUpAmmo_Implementation(FString AmmoName, int Amount);
	//virtual void PickUpWeapon_Implementation(FName weaponName, int primaryAmountOfAmmo, int primaryAmmoInTheClip, int secondaryAmountOfAmmo, int secondaryAmmoInTheClip) override;

	float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)override;

	/*returns true if actually died, false otherwise(for exapmle god mode ws on)*/
	UFUNCTION(BlueprintCallable)
		virtual bool OnDeath();

protected:
	virtual void BeginPlay();

	void OnCrouch();

	void OnUnCrouch();

public:

	

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;



	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AuhlProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		uint32 bUsingMotionControllers : 1;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Mesh)
		class USceneComponent* VR_MuzzleLocation;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Mesh)
		class USceneComponent* FP_MuzzleLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = Sound)
		USoundBase* LadderClimbingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = Sound)
		USoundBase* DeathSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = Sound)
		USoundBase*FireBurnSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = Sound)
		USoundBase* DrownSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = Sound)
		USoundBase* FallPainSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = Sound)
		USoundBase* PosionPainSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = Sound)
		USoundBase* SprintSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = Sound)
		USoundBase* GeigerSound;


	/** Sound to play each time we start sprinting */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = Sound)
		class USoundBase* SprintingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		bool bHoldingItem = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		bool bPhysicsHandleActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		APhysicalActorBase* PhysicsObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		UPhysicsHandleComponent* PhysicsHandle;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = Sound)
		float TimeSinceLastStep = 0.0;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = Sound)
		float TimePerLastStep = 0.5;

	float DefaultTimePerLastStep = 0.5;

	UFUNCTION(BlueprintPure)
		float GetDefaultTimePerLastStep()const { return DefaultTimePerLastStep; }


protected:
	virtual void Tick(float DeltaTime) override;

	/**/
	void OnUse();

	void OnStopUse();

	void ScrollUp();

	void ScrollDown();

	/** Fires a projectile. */
	void OnPrimaryFire();

	void OnSecondaryFire();

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false; Location = FVector::ZeroVector; }
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/*
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

