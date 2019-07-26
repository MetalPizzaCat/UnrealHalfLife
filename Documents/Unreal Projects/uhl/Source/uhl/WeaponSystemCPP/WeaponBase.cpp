// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
//#include "WeaponFireComponents/BaseFireComponent.h"
#include "Engine.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTabe'/Game/DataTables/DT_WeaponsData.DT_WeaponsData'"));
	if (DataTable.Succeeded())
	{
		this->WeaponDataTable = DataTable.Object;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Failed to load weapon data table"));
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> AnimationDataTable(TEXT("DataTabe'/Game/DataTables/DT_WeaponAnimsData.DT_WeaponAnimsData'"));
	if (AnimationDataTable.Succeeded())
	{
		this->AnimationDataTable = AnimationDataTable.Object;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Failed to load weapon animation data table"));
	}

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponSkeletalMesh"));

	WeaponMesh->SetVisibility(this->CanBePickedUp, true);	
	WeaponMesh->BodyInstance.SetCollisionProfileName("PhysicsActor");
	WeaponMesh->SetNotifyRigidBodyCollision(true);
	//WeaponMesh->OnComponentHit.AddDynamic(this, &AWeaponBase::OnMeshHit);

	RootComponent = WeaponMesh;
}

bool AWeaponBase::PrimaryFire(FVector location, FRotator rotation)
{
	
	if (this->Data.PrimaryFireComponent != NULL)
	{
		if (this->Data.Melee)
		{
			if (this->CanShoot(EFireType::FTE_Primary))
			{
				this->Data.PrimaryFireComponent->GetDefaultObject<UBaseFireComponent>()->Fire(EFireType::FTE_Primary, location, rotation, this);
				this->PrimaryIsShooting = true;
				return true;
			}
		}
		else
		{
			if (this->CanShoot(EFireType::FTE_Primary))
			{
				if (this->PrimaryAmmoInTheClip > 0)
				{

					this->Data.PrimaryFireComponent->GetDefaultObject<UBaseFireComponent>()->Fire(EFireType::FTE_Primary, location, rotation, this);
					this->PrimaryIsShooting = true;
					this->PrimaryAmmoInTheClip--;
					this->AbortPrimaryReload();


					return true;
					/*if (!this->Data.OnlyWorldModel && this->WeaponMesh != NULL && this->AnimationData != NULL)
					{
						int max = this->AnimationData->Fire.Num();
						int res = 0;
						if (max > 0)
						{
							res = FMath::RandRange(0, max);
						}
						if (this->AnimationData->Fire[res] != NULL)
						{
							this->WeaponMesh->PlayAnimation(this->AnimationData->Fire[res], false);
						}
					}*/
				}
				else if (this->PrimaryAmountOfAmmo > 0)
				{
					this->PrimaryReload();
					return false;
				}
				else
				{
					return false;
					/*if (!this->Data.OnlyWorldModel && this->WeaponMesh != NULL && this->AnimationData != NULL)
					{

						if (this->AnimationData->FireEmpty != NULL)
						{
							this->WeaponMesh->PlayAnimation(this->AnimationData->FireEmpty, false);
						}
					}*/
				}
			}
		}
	}
		return false;
	
	return false;
}

bool AWeaponBase::SecondaryFire(FVector location, FRotator rotation)
{
	
	if (this->Data.SecondaryFireComponent != NULL)
	{
		if (this->Data.Melee)
		{
			if(this->CanShoot(EFireType::FTE_Secondary))
			{
				this->Data.SecondaryFireComponent->GetDefaultObject<UBaseFireComponent>()->Fire(EFireType::FTE_Secondary, location, rotation, this);
				this->SecondaryIsShooting = true;
				return true;
			}
		}
		else 
		{
			if (this->CanShoot(EFireType::FTE_Secondary))
			{
				if (this->Data.SecondaryFireUsesPrimaryClip)
				{

					if (this->PrimaryAmmoInTheClip > 0)
					{
						this->Data.SecondaryFireComponent->GetDefaultObject<UBaseFireComponent>()->Fire(EFireType::FTE_Secondary, location, rotation, this);
						this->PrimaryIsShooting = true;
						this->PrimaryAmmoInTheClip--;
						this->AbortSecondaryReload();
						return true;

					}
					else if (this->PrimaryAmountOfAmmo > 0)
					{
						this->PrimaryReload();
						return false;
					}
					else
					{
						return false;

					}
				}
				else {
					if (this->SecondaryAmmoInTheClip > 0)
					{
						this->Data.SecondaryFireComponent->GetDefaultObject<UBaseFireComponent>()->Fire(EFireType::FTE_Secondary, location, rotation, this);
						this->SecondaryIsShooting = true;
						this->SecondaryAmmoInTheClip--;
						return true;

					}
					else if (this->SecondaryAmountOfAmmo > 0)
					{
						this->SecondaryReload();
						return false;
					}
					else
					{
						return false;

					}
				}
			}
		}
	}
		return false;
	
	return false;
}

void AWeaponBase::EndPrimaryReload()
{
	if (this->Data.PrimaryReloadType == EReloadType::RTE_Clip)
	{
		if ((this->PrimaryAmountOfAmmo - this->Data.PrimaryAmmoPerClip + this->PrimaryAmmoInTheClip) >= 0)
		{
			this->PrimaryAmountOfAmmo -= this->Data.PrimaryAmmoPerClip - this->PrimaryAmmoInTheClip;
			this->PrimaryAmmoInTheClip = this->Data.PrimaryAmmoPerClip;
			if (this->WeaponOwner != NULL)
			{
				if (this->WeaponOwner->IsValidLowLevel() && Cast<IWeaponInterface>(this->WeaponOwner) != NULL)
				{
					IWeaponInterface::Execute_OwnerNotifyEndPrimaryReload(this->WeaponOwner);
				}
			}

		}
		else if (this->PrimaryAmountOfAmmo > 0)
		{
			this->PrimaryAmmoInTheClip = this->PrimaryAmountOfAmmo;
			this->PrimaryAmountOfAmmo = 0;
			if (this->WeaponOwner != NULL)
			{
				if (this->WeaponOwner->IsValidLowLevel() && Cast<IWeaponInterface>(this->WeaponOwner) != NULL)
				{
					IWeaponInterface::Execute_OwnerNotifyEndPrimaryReload(this->WeaponOwner);
				}
			}
		}
	}
	else if (this->Data.PrimaryReloadType == EReloadType::RTE_OneBullet)
	{
		if ((this->PrimaryAmountOfAmmo - 1) >= 0 && this->PrimaryAmmoInTheClip < this->Data.PrimaryAmmoPerClip)
		{
			this->PrimaryAmountOfAmmo -= 1;
			this->PrimaryAmmoInTheClip++;
			if (this->WeaponOwner != NULL)
			{
				if (this->WeaponOwner->IsValidLowLevel() && Cast<IWeaponInterface>(this->WeaponOwner) != NULL)
				{
					IWeaponInterface::Execute_OwnerNotifyEndPrimaryReload(this->WeaponOwner);
				}
			}

		}
	}
}


void AWeaponBase::EndSecondaryReload()
{
	if (this->WeaponOwner != NULL)
	{
		if (this->WeaponOwner->IsValidLowLevel() && Cast<IWeaponInterface>(this->WeaponOwner) != NULL)
		{
			IWeaponInterface::Execute_OwnerNotifyEndSecondaryReload(this->WeaponOwner);
		}
	}
}


bool AWeaponBase::Fire(EFireType type, FVector location, FRotator rotation)
{
	if (type == EFireType::FTE_Primary) { return this->PrimaryFire(location, rotation); }
	else if(type == EFireType::FTE_Secondary) { return this->SecondaryFire(location, rotation); }
	return false;
}

void AWeaponBase::PrimaryReload()
{
	if (!this->IsReloading)
	{
		this->IsReloading = true;
		this->TimeInReload = 0;
		if (this->WeaponOwner != NULL)
		{
			if (this->WeaponOwner->IsValidLowLevel() && Cast<IWeaponInterface>(this->WeaponOwner) != NULL)
			{
				IWeaponInterface::Execute_OwnerNotifyPrimaryReload(this->WeaponOwner);
			}
		}
	}
}

void AWeaponBase::SecondaryReload()
{
	/*Because this is half-life reloading of alt fire doesn't really happens. Gordon magicly puts new grenade info the smg(also - Can this smg shoot grenades in real life?)*/
	if ((this->SecondaryAmountOfAmmo - this->Data.SecondaryAmmoPerClip) >= 0)
	{
		this->SecondaryAmountOfAmmo -= this->Data.SecondaryAmmoPerClip;
		this->SecondaryAmmoInTheClip = this->Data.SecondaryAmmoPerClip;
		if (this->WeaponOwner != NULL)
		{
			if (this->WeaponOwner->IsValidLowLevel() && Cast<IWeaponInterface>(this->WeaponOwner) != NULL)
			{
				IWeaponInterface::Execute_OwnerNotifySecondaryReload(this->WeaponOwner);
			}
		}
	}
}


void AWeaponBase::Reload(EFireType type)
{
	if (type == EFireType::FTE_Primary) { this->PrimaryReload(); }
	else if (type == EFireType::FTE_Secondary) { this->SecondaryReload(); }
}

void AWeaponBase::AbortPrimaryReload()
{
	this->TimeInReload = 0.0f;
	this->IsReloading = false;
}

void AWeaponBase::AbortSecondaryReload()
{
	/*Because this is half-life reloading of alt fire doesn't really happens. Gordon magicly puts new grenade info the smg(also - Can this smg shoot grenades in real life?)*/
	
}

void AWeaponBase::AbortReload(EFireType type)
{
	if (type == EFireType::FTE_Primary) { this->AbortPrimaryReload(); }
	else if (type == EFireType::FTE_Secondary) { this->AbortSecondaryReload(); }
}

bool AWeaponBase::CanShoot(EFireType type)
{
	if (this->Data.Melee)
	{
		if (type == EFireType::FTE_Primary)
		{
			return (!this->PrimaryIsShooting);
		}
		if (type == EFireType::FTE_Secondary)
		{
			return (!this->SecondaryIsShooting);
		}
	}
	else
	{
		if (type == EFireType::FTE_Primary)
		{
			return (this->Data.PrimaryReloadType == EReloadType::RTE_Clip) ? (!this->PrimaryIsShooting) && (this->PrimaryAmmoInTheClip > 0) && (!this->IsReloading) : (!this->PrimaryIsShooting) && (this->PrimaryAmmoInTheClip > 0);
		}
		if (type == EFireType::FTE_Secondary)
		{
			return (!this->SecondaryIsShooting) && (this->SecondaryAmmoInTheClip > 0);
		}
	}
	return false;
}

bool AWeaponBase::LoadData(UDataTable* weaponDT, UDataTable* animationDT)
{
	if (weaponDT == NULL)
	{
		return false;
	}

	if (animationDT == NULL)
	{
		return false;
	}

	bool res = false;
	static const FString ContextString(TEXT("DT_WeaponData"));
	TArray<FWeaponData*> array;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, this->Name.ToString());
	if (weaponDT != NULL)
	{
		weaponDT->GetAllRows<FWeaponData>(ContextString, array);
		for (int i = 0; i < array.Num(); i++)
		{
			if (array[i]->Name == this->Name.ToString()) { this->Data = array[i]->Get(); }
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, array[i]->Name);
		}
		if (this->Data.DisplayName == "null") { res = false; GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Failed to load weapon data")); }
		else { res = true; }
		//else if (this->Data.Mesh!=NULL){ this->WeaponMesh->SkeletalMesh = this->Data.Mesh; }
		//UGameplayStatics::PlaySoundAtLocation(this, this->Data.PrimaryFireSound, this->GetActorLocation());
	}

	static const FString AnimContextString(TEXT("DT_AnimData"));
	TArray<FWeaponAnimData*> AnimationArray;
	if (animationDT != NULL)
	{
		animationDT->GetAllRows<FWeaponAnimData>(AnimContextString, AnimationArray);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, this->Data.AnimationDataName);
		for (int i = 0; i < AnimationArray.Num(); i++)
		{
			if (AnimationArray[i]->Name == this->Data.AnimationDataName) { this->AnimationData = AnimationArray[i]; }
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, AnimationArray[i]->Name);
		}
		if (this->AnimationData == NULL) { GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Failed to load animation data")); }
		else { res = true; }
		//UGameplayStatics::PlaySoundAtLocation(this, this->Data.PrimaryFireSound, this->GetActorLocation());
	}

	return res;
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	WeaponMesh->SetVisibility(true, true);
	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, this->Name.ToString());
	}
	static const FString ContextString(TEXT("DT_WeaponData"));
	if (data != NULL)
	{
		this->Data = this->WeaponDataTable->FindRow<FWeaponData>(this->Name, ContextString)->Get();
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Failed to load data"));
		}
	}*/
	static const FString ContextString(TEXT("DT_WeaponData"));
	TArray<FWeaponData*> array;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, this->Name.ToString());
	if (this->WeaponDataTable != NULL)
	{
		this->WeaponDataTable->GetAllRows<FWeaponData>(ContextString, array);
		for (int i = 0; i < array.Num(); i++)
		{
			if (array[i]->Name == this->Name.ToString()) { this->Data = array[i]->Get(); }
		}
		if (this->Data.DisplayName == "null") { GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Failed to load weapon data")); }
		//else if (this->Data.Mesh!=NULL){ this->WeaponMesh->SkeletalMesh = this->Data.Mesh; }
		//UGameplayStatics::PlaySoundAtLocation(this, this->Data.PrimaryFireSound, this->GetActorLocation());
	}

	static const FString AnimContextString(TEXT("DT_AnimData"));
	TArray<FWeaponAnimData*> AnimationArray;
	if (this->AnimationDataTable != NULL)
	{
		this->AnimationDataTable->GetAllRows<FWeaponAnimData>(AnimContextString, AnimationArray);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, this->Data.AnimationDataName);
		for (int i = 0; i < AnimationArray.Num(); i++)
		{
			if (AnimationArray[i]->Name == this->Data.AnimationDataName) { this->AnimationData = AnimationArray[i]; }
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, AnimationArray[i]->Name);
		}
		if (this->AnimationData == NULL) { GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Failed to load animation data")); }
		//UGameplayStatics::PlaySoundAtLocation(this, this->Data.PrimaryFireSound, this->GetActorLocation());
	}
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (this->PrimaryIsShooting)
	{
		this->PrimaryTimeSinceLastShot += DeltaTime;
		if (this->PrimaryTimeSinceLastShot > this->Data.PrimaryTimeBetweenShots)
		{
			this->PrimaryTimeSinceLastShot = 0;
			this->PrimaryIsShooting = false;
		}
	}
	if (this->SecondaryIsShooting)
	{
		this->SecondaryTimeSinceLastShot += DeltaTime;
		if (this->SecondaryTimeSinceLastShot > this->Data.SecondaryTimeBetweenShots)
		{
			this->SecondaryTimeSinceLastShot = 0;
			this->SecondaryIsShooting = false;
		}
	}
	if (this->IsReloading)
	{
		this->TimeInReload += DeltaTime;
		if (this->TimeInReload >= this->Data.PrimaryTimeToReload)
		{
			this->TimeInReload = 0.0f;
			this->IsReloading = false;
			this->EndPrimaryReload();
		}
	}
	if (!this->PrimaryIsShooting && !this->SecondaryIsShooting && !this->IsReloading)
	{
		/*if (!this->Data.OnlyWorldModel && this->Data.Mesh != NULL && this->AnimationData != NULL)
		{

			if (this->AnimationData->Idle!= NULL)
			{
				this->WeaponMesh->PlayAnimation(this->AnimationData->Idle, false);
			}
		}*/
	}
}

void AWeaponBase::OnMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Weapon is touched"));
	IWeaponInterface::Execute_PickUpWeapon(OtherActor, this->WeaponDataTable,this->AnimationDataTable,this->Name, this->PrimaryAmountOfAmmo, this->PrimaryAmmoInTheClip, this->SecondaryAmountOfAmmo, this->SecondaryAmmoInTheClip);
}

