// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "uhlCharacter.h"
#include "uhlProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Physics/PhysicsMaterial.h"
#include "Engine.h"


#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AuhlCharacter


AuhlCharacter::AuhlCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(45.f, 90.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.


	this->PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	if (PhysicsHandle == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("ERROR: PhysicsHandle is nullptr"));
	}
	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;

	PrimaryActorTick.bCanEverTick = true;
}

bool AuhlCharacter::PickUpWeapon_Implementation(UDataTable* WeaponDataTable, UDataTable* AnimationDataTable, FName weaponName, int primaryAmountOfAmmo, int primaryAmmoInTheClip, int secondaryAmountOfAmmo, int secondaryAmmoInTheClip)
{
	
	for (int i = 0; i < this->Weapons.Num(); i++)
	{
		if (this->Weapons[i]->Name == weaponName) { return false ; }
	}
	if (this->GetWorld() != NULL)
	{
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		this->Weapons.Add(this->GetWorld()->SpawnActor<AWeaponBase>(this->GetActorLocation(), this->GetActorRotation(), ActorSpawnParams));
		this->Weapons[this->Weapons.Num() - 1]->Name = weaponName;
		if (this->Weapons[this->Weapons.Num() - 1]->LoadData(WeaponDataTable, AnimationDataTable))
		{
			this->Weapons[this->Weapons.Num() - 1]->PrimaryAmmoInTheClip = primaryAmmoInTheClip;
			this->Weapons[this->Weapons.Num() - 1]->PrimaryAmountOfAmmo = primaryAmountOfAmmo;
			this->Weapons[this->Weapons.Num() - 1]->SecondaryAmmoInTheClip = secondaryAmmoInTheClip;
			this->Weapons[this->Weapons.Num() - 1]->SecondaryAmountOfAmmo = secondaryAmountOfAmmo;
			this->Weapons[this->Weapons.Num() - 1]->WeaponOwner = this;
			return true;
		}
		else
		{
			this->Weapons.RemoveAt<int>(this->Weapons.Num() - 1, 1, true);
			return false;
		}
	}

	return false;
}

bool AuhlCharacter::OnDeath()
{
	if (GetWorld() != nullptr && this->DeathSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), this->DeathSound, this->GetActorLocation());
	}
	this->Dead = true;

	return true;
}

void AuhlCharacter::BeginPlay()
{
	
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}
	
	this->UnCrouch();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AuhlCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind sprintp events
	PlayerInputComponent->BindAction("Spint", IE_Pressed, this, &AuhlCharacter::StartSprinting);
	PlayerInputComponent->BindAction("Spint", IE_Released, this, &AuhlCharacter::StopSprinting);

	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryFire", IE_Pressed, this, &AuhlCharacter::OnPrimaryFire);
	PlayerInputComponent->BindAction("SecondaryFire", IE_Pressed, this, &AuhlCharacter::OnSecondaryFire);

	PlayerInputComponent->BindAction("ScrollUp", IE_Pressed, this, &AuhlCharacter::ScrollUp);
	PlayerInputComponent->BindAction("ScrollDown", IE_Pressed, this, &AuhlCharacter::ScrollDown);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AuhlCharacter::OnCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AuhlCharacter::OnUnCrouch);

	//Bind Use event
	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &AuhlCharacter::OnUse);
	PlayerInputComponent->BindAction("Use", IE_Released, this, &AuhlCharacter::OnStopUse);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AuhlCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AuhlCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AuhlCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AuhlCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AuhlCharacter::LookUpAtRate);
}

void AuhlCharacter::OnCrouch()
{
	if (!this->Dead)
	{
		if (this->CanCrouch())
		{
			if (this->bSprinting) { this->StopSprinting(); }
			this->TimePerLastStep *= 2;
			this->Crouch();
		}
	}
}

void AuhlCharacter::OnUnCrouch()
{
	this->UnCrouch();
	this->TimePerLastStep = this->GetDefaultTimePerLastStep();
}

void AuhlCharacter::OnUse()
{
	if (!this->Dead)
	{


		FVector start = this->FP_Gun->GetComponentLocation();

		FVector end = start + (this->FirstPersonCameraComponent->GetForwardVector() * 400);

		FHitResult hitResult;
		FCollisionQueryParams queryParams = FCollisionQueryParams::DefaultQueryParam;
		queryParams.AddIgnoredActor(this);

		this->GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECollisionChannel::ECC_Camera, queryParams);
		if (hitResult.Actor.IsValid() && hitResult.Actor->GetClass()->ImplementsInterface(UCI_Interact::StaticClass()))
		{
			ICI_Interact::Execute_BeUsed(hitResult.Actor.Get(), this);
		}
		else if (hitResult.Actor.IsValid() && Cast<APhysicalActorBase>(hitResult.GetActor()) != nullptr)
		{
			if (PhysicsHandle != nullptr)
			{
				if (hitResult.GetComponent() != nullptr)
				{
					this->bHoldingItem = true;
					//hitResult.GetActor()->GetRootComponent()->ComponentVelocity = FVector(0, 0, 0);
					PhysicsHandle->GrabComponent(hitResult.GetComponent(), "None", hitResult.Location, true);

					PhysicsObject = Cast<APhysicalActorBase>(hitResult.GetActor());
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("hitResult.GetComponent() returns nullptr"));
				}
			}
			else
			{
				
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("ERROR: PhysicsHandle is nullptr"));
			}
		}
		if (hitResult.Location != FVector::ZeroVector)
		{
			DrawDebugLine(this->GetWorld(), start, hitResult.Location, FColor::Green, false, 5.f, 0, 1.f);
		}
		else
		{
			DrawDebugLine(this->GetWorld(), start, end, FColor::Red, false, 5.f, 0, 1.f);
		}

		this->bPhysicsHandleActive = !this->bPhysicsHandleActive;
		if (!this->bPhysicsHandleActive && PhysicsHandle != nullptr)
		{

			PhysicsHandle->ReleaseComponent();
			this->bHoldingItem = false;
			PhysicsObject = nullptr;
		}
	}
}

void AuhlCharacter::OnStopUse()
{
}

void AuhlCharacter::OnPrimaryFire()
{
	// try and fire a projectile
	//if (ProjectileClass != NULL)
	//{
	//	UWorld* const World = GetWorld();
	//	if (World != NULL)
	//	{
	//		if (bUsingMotionControllers)
	//		{
	//			const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
	//			const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
	//			World->SpawnActor<AuhlProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
	//		}
	//		else
	//		{
	//			const FRotator SpawnRotation = GetControlRotation();
	//			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	//			const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);
	//			//Set Spawn Collision Handling Override
	//			FActorSpawnParameters ActorSpawnParams;
	//			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	//			// spawn the projectile at the muzzle
	//			World->SpawnActor<AuhlProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	//		}
	//	}
	//}

	if (!this->Dead)
	{
		if (this->bPhysicsHandleActive && this->bHoldingItem && this->PhysicsObject != nullptr)
		{
			this->bPhysicsHandleActive = false;

			PhysicsHandle->GrabbedComponent->AddImpulse(this->GetFirstPersonCameraComponent()->GetForwardVector() * 500);
			PhysicsHandle->ReleaseComponent();
			this->bHoldingItem = false;
			PhysicsObject = nullptr;

		}
		else 
		{
			if (this->Weapons[this->CurrentWeaponId] != NULL)
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);
				this->Weapons[this->CurrentWeaponId]->PrimaryFire(SpawnLocation, SpawnRotation);
			}
			else
			{
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("ERROR:Character is trying to use Weapon, but it is NULL"));
				}
			}
		}
	}
	// try and play the sound if specified
	//if (FireSound != NULL)
	//{
	//	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	//}

	//// try and play a firing animation if specified
	//if (FireAnimation != NULL)
	//{
	//	// Get the animation object for the arms mesh
	//	UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
	//	if (AnimInstance != NULL)
	//	{
	//		AnimInstance->Montage_Play(FireAnimation, 1.f);
	//	}
	//}
}

void AuhlCharacter::OnSecondaryFire()
{

	if (!this->Dead)
	{
		if (this->bPhysicsHandleActive && this->bHoldingItem && this->PhysicsObject != nullptr)
		{
			this->bPhysicsHandleActive = false;

			PhysicsHandle->ReleaseComponent();
			this->bHoldingItem = false;
			PhysicsObject = nullptr;

		}
		else {
			if (this->Weapons[this->CurrentWeaponId] != NULL)
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);
				this->Weapons[this->CurrentWeaponId]->SecondaryFire(SpawnLocation, SpawnRotation);
			}
			else
			{
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("ERROR:Character is trying to use Weapon, but it is NULL"));
				}
			}

			// try and play the sound if specified
			if (FireSound != NULL)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			// try and play a firing animation if specified
			if (FireAnimation != NULL)
			{
				// Get the animation object for the arms mesh
				UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
				if (AnimInstance != NULL)
				{
					AnimInstance->Montage_Play(FireAnimation, 1.f);
				}
			}
		}
	}
}

void AuhlCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AuhlCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnPrimaryFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AuhlCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AuhlCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void AuhlCharacter::StartUsingLadder()
{
	if (!this->Dead)
	{
		this->UsingLadder = true;
		this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		this->GetRootComponent()->ComponentVelocity = FVector(0, 0, 0);
	}
}

void AuhlCharacter::StopUsingLadder()
{
	this->UsingLadder = false;
	this->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AuhlCharacter::StartSprinting()
{
	/*Can not sprint while crouching*/
	if (!this->GetMovementComponent()->IsCrouching() && this->bCanSprint)
	{
		this->bSprinting = true;

		Cast<UCharacterMovementComponent>(this->GetMovementComponent())->MaxWalkSpeed = this->GetMaxSpeedSprinting();

		if (GetWorld() != nullptr && this->SprintingSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), this->SprintingSound, this->GetActorLocation());
		}

		this->TimePerLastStep /= 2;
	}
}

void AuhlCharacter::StopSprinting()
{
	if (this->bSprinting)
	{
		this->bSprinting = false;

		Cast<UCharacterMovementComponent>(this->GetMovementComponent())->MaxWalkSpeed = 600;/*This value is default for every character and as not changed for this project*/

		this->TimePerLastStep = this->GetDefaultTimePerLastStep();
	}
}

void AuhlCharacter::MoveForward(float Value)
{
	if (!this->Dead)
	{
		if (Value != 0.0f)
		{
			if (this->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying || this->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Swimming)
			{
				AddMovementInput(FVector(0, 0, 100), Value);

				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, this->FirstPersonCameraComponent->GetForwardVector().Rotation().ToString());
			}
			else
			{
				// add movement in that direction
				AddMovementInput(GetActorForwardVector(), Value);
			}
		}
	}
}

void AuhlCharacter::MoveRight(float Value)
{
	if (!this->Dead)
	{
		if (Value != 0.0f)
		{
			// add movement in that direction
			AddMovementInput(GetActorRightVector(), Value);
		}
	}
}

void AuhlCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AuhlCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AuhlCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AuhlCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AuhlCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AuhlCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

void  AuhlCharacter::ScrollUp()
{
	int temp = this->CurrentWeaponId;
	this->CurrentWeaponId--;
	if (this->CurrentWeaponId < 0) { this->CurrentWeaponId = 0; }
	this->WeaponChageAnimNote(temp, this->CurrentWeaponId);

	if (this->Ammo.Contains(this->GetCurrentWeapon()->Data.PrimaryAmmoType))
	{
		this->Weapons[this->CurrentWeaponId]->PrimaryAmountOfAmmo = this->Ammo.FindChecked(this->GetCurrentWeapon()->Data.PrimaryAmmoType);
	}
	if (this->Ammo.Contains(this->GetCurrentWeapon()->Data.SecondaryAmmoType))
	{
		this->Weapons[this->CurrentWeaponId]->SecondaryAmountOfAmmo = this->Ammo.FindChecked(this->GetCurrentWeapon()->Data.SecondaryAmmoType);
	}
}

void  AuhlCharacter::ScrollDown()
{
	int temp = this->CurrentWeaponId;
	this->CurrentWeaponId++;
	if (this->CurrentWeaponId < this->Weapons.Num()) {}
	else
	{
		this->CurrentWeaponId = this->Weapons.Num() - 1;
	}
	this->WeaponChageAnimNote(temp, this->CurrentWeaponId);

	if (this->Ammo.Contains(this->GetCurrentWeapon()->Data.PrimaryAmmoType))
	{
		this->Weapons[this->CurrentWeaponId]->PrimaryAmountOfAmmo = this->Ammo.FindChecked(this->GetCurrentWeapon()->Data.PrimaryAmmoType);
	}
	if (this->Ammo.Contains(this->GetCurrentWeapon()->Data.SecondaryAmmoType))
	{
		this->Weapons[this->CurrentWeaponId]->SecondaryAmountOfAmmo = this->Ammo.FindChecked(this->GetCurrentWeapon()->Data.SecondaryAmmoType);
	}
}

AWeaponBase* AuhlCharacter::GetCurrentWeapon()
{
	

	if (this->Weapons.Num() != 0)
	{
		return this->Weapons[this->CurrentWeaponId];
	}
	else
	{
		return nullptr;
	}
}

bool  AuhlCharacter::PickUpAmmo_Implementation(FString AmmoName, int Amount)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, AmmoName);
	if (this->Ammo.Contains(AmmoName))
	{
		this->Ammo.Add(AmmoName, Amount + this->Ammo.FindChecked(AmmoName));
		for (int i = 0; i < this->Weapons.Num(); i++)
		{
			if (this->Ammo.Contains(this->Weapons[i]->Data.PrimaryAmmoType))
			{
				this->Weapons[i]->PrimaryAmountOfAmmo = this->Ammo.FindChecked(this->Weapons[i]->Data.PrimaryAmmoType);
			}
			if (this->Ammo.Contains(this->Weapons[i]->Data.SecondaryAmmoType))
			{
				this->Weapons[i]->SecondaryAmountOfAmmo = this->Ammo.FindChecked(this->Weapons[i]->Data.SecondaryAmmoType);
			}
		}
		return true;
	}
	else
	{
		this->Ammo.Add(AmmoName, Amount);
		for (int i = 0; i < this->Weapons.Num(); i++)
		{
			if (this->Ammo.Contains(this->Weapons[i]->Data.PrimaryAmmoType))
			{
				this->Weapons[i]->PrimaryAmountOfAmmo = this->Ammo.FindChecked(this->Weapons[i]->Data.PrimaryAmmoType);
			}
			if (this->Ammo.Contains(this->Weapons[i]->Data.SecondaryAmmoType))
			{
				this->Weapons[i]->SecondaryAmountOfAmmo = this->Ammo.FindChecked(this->Weapons[i]->Data.SecondaryAmmoType);
			}
		}
		return true;
	}
	
	return false;

}

void AuhlCharacter::Tick(float DeltaTime)
{
	if (this->bPhysicsHandleActive && this->bHoldingItem && this->PhysicsObject != nullptr)
	{
		this->PhysicsHandle->SetTargetLocation(this->GetFirstPersonCameraComponent()->GetComponentLocation() + (this->GetFirstPersonCameraComponent()->GetForwardVector() * 300));
		//this->PhysicsObject->SetActorLocation(this->GetFirstPersonCameraComponent()->GetComponentLocation() + (this->GetFirstPersonCameraComponent()->GetForwardVector() * 300));
	}

	for (int i = 0; i < this->Weapons.Num(); i++)
	{
		this->Ammo.Add(this->Weapons[i]->Data.PrimaryAmmoType, this->Weapons[i]->PrimaryAmountOfAmmo);

		this->Ammo.Add(this->Weapons[i]->Data.SecondaryAmmoType, this->Weapons[i]->SecondaryAmountOfAmmo);
	}
	this->TimeSinceLastStep += DeltaTime;
	if (this->TimeSinceLastStep >= TimePerLastStep)
	{
		this->TimeSinceLastStep = 0;
		if (GetWorld() != nullptr && this->GetVelocity() != FVector(0, 0, 0))
		{
			if (this->UsingLadder)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), this->LadderClimbingSound, this->GetActorLocation());
			}
			else
			{

				FHitResult hitResult;
				FCollisionQueryParams queryParams = FCollisionQueryParams::DefaultQueryParam;
				queryParams.AddIgnoredActor(this);

				GetWorld()->LineTraceSingleByChannel(hitResult, GetCapsuleComponent()->GetComponentLocation(), GetCapsuleComponent()->GetComponentLocation() - FVector(0, 0, 200), ECollisionChannel::ECC_Camera, queryParams);

				if (hitResult.bBlockingHit)
				{
					if (hitResult.PhysMaterial != nullptr && Cast<UPhysicsMaterial>(hitResult.PhysMaterial) != nullptr && Cast<UPhysicsMaterial>(hitResult.PhysMaterial)->StepSound != nullptr)
					{
						/*if (Cast<UPhysicsMaterial>(hitResult.PhysMaterial) != nullptr)
						{
							if (Cast<UPhysicsMaterial>(hitResult.PhysMaterial)->StepSound != nullptr)
							{*/
						UGameplayStatics::PlaySoundAtLocation(GetWorld(), Cast<UPhysicsMaterial>(hitResult.PhysMaterial)->StepSound, hitResult.Location);
						/*	}
						}*/
					}
					else if (hitResult.GetComponent() != nullptr)
					{
						if (hitResult.GetComponent()->GetMaterial(0) != nullptr)
						{
							if (hitResult.GetComponent()->GetMaterial(0)->GetPhysicalMaterial() != nullptr)
							{


								if (Cast<UPhysicsMaterial>(hitResult.GetComponent()->GetMaterial(0)->GetPhysicalMaterial()) != nullptr)
								{
									if (Cast<UPhysicsMaterial>(hitResult.GetComponent()->GetMaterial(0)->GetPhysicalMaterial())->BulletHitSound != nullptr)
									{
										UGameplayStatics::PlaySoundAtLocation(GetWorld(), Cast<UPhysicsMaterial>(hitResult.GetComponent()->GetMaterial(0)->GetPhysicalMaterial())->StepSound, hitResult.Location);

									}
								}
							}
						}
					}
				}
			}
		}
	}

	AnimUpdateTick(DeltaTime);
	
	if (this->bSprinting)
	{
		this->SprintedTime += DeltaTime;
		if (this->SprintedTime >= this->GetMaxSprintTime())
		{
			this->bCanSprint = false;
			this->StopSprinting();
		}
	}
	else
	{
		if (this->SprintedTime > 0)
		{
			this->SprintedTime -= DeltaTime;
			if (this->SprintedTime < 0) { this->SprintedTime = 0; this->bCanSprint = true; }
		}
	}
}

float AuhlCharacter::GetMaxHealth()const
{
	return MaxHealth;
}

float  AuhlCharacter::GetMaxSuitEnergy() const
{
	return this->IsSuitOn ? MaxSuitEnergy : 0;
}

float AuhlCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	// Call the base class - this will tell us how much damage to apply  
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (!this->Dead)
	{
		if (DamageEvent.DamageTypeClass->GetName() == "BPDT_Fire_C")
		{
			if (GetWorld() != nullptr && this->FireBurnSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), this->FireBurnSound, this->GetActorLocation());
			}
		}
		if (DamageEvent.DamageTypeClass->GetName() == "BPDT_Radiation_C")
		{
			if (GetWorld() != nullptr && this->GeigerSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), this->GeigerSound, this->GetActorLocation());
			}
		}
		else if (DamageEvent.DamageTypeClass->GetName() == "BPDT_Poison_C")
		{
			if (GetWorld() != nullptr && this->PosionPainSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), this->PosionPainSound, this->GetActorLocation());
			}
		}
		else if (DamageEvent.DamageTypeClass->GetName() == "BPDT_Fall_C")
		{
			if (GetWorld() != nullptr && this->FallPainSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), this->FallPainSound, this->GetActorLocation());
			}
		}
		else if (DamageEvent.DamageTypeClass->GetName() == "BPDT_Drown_C")
		{
			if (GetWorld() != nullptr && this->DrownSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), this->DrownSound, this->GetActorLocation());
			}
		}

		if (this->IsSuitOn && this->SuitEnergy > 0 && DamageEvent.DamageTypeClass->GetName() != "BPDT_Radiation_C" && DamageEvent.DamageTypeClass->GetName() != "BPDT_Poison_C" && DamageEvent.DamageTypeClass->GetName() != "BPDT_Fall_C" && DamageEvent.DamageTypeClass->GetName() != "BPDT_Drown_C")
		{


			float dmgHealth = Damage / 5;

			float dmgSuit = Damage - Damage / 5;

			if (dmgSuit > this->SuitEnergy)
			{
				/*Store value of damage to return */
				float dmg = dmgHealth + this->SuitEnergy;

				this->Health -= dmgHealth - this->SuitEnergy;
				this->SuitEnergy = 0;

				if (this->Health <= 0)
				{
					this->OnDeath();
				}

				return dmg;
			}
			else
			{
				this->Health -= dmgHealth;
				this->SuitEnergy -= dmgSuit;

				if (this->Health <= 0)
				{
					this->OnDeath();
				}

				return dmgHealth;
			}
		}
		else
		{

			this->Health -= Damage;

			if (this->Health <= 0)
			{
				this->OnDeath();
			}

			return Damage;
		}
	}
	else
	{
		return 0;
	}

}