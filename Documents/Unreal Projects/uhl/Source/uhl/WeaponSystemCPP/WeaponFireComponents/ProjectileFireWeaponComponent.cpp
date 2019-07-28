// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileFireWeaponComponent.h"
#include "WeaponSystemCPP/Projectiles/BaseProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Engine.h"

void UProjectileFireWeaponComponent::Fire(EFireType type, FVector location, FRotator rotation, AWeaponBase* weapon)
{

	if (weapon->GetWorld() != NULL && weapon->WeaponOwner != nullptr)
	{
		float CurrentFiringSpread = ((type == EFireType::FTE_Primary) ? weapon->Data.PrimarySpread : weapon->Data.SecondarySpread);
		int count = (type == EFireType::FTE_Primary) ? weapon->Data.PrimaryBulletsPerShot : weapon->Data.PrimaryBulletsPerShot;
		for (int j = 0; j < count; j++)
		{
			const int32 RandomSeed = FMath::Rand();
			FRandomStream WeaponRandomStream(RandomSeed);
			//float CurrentSpread = GetCurrentSpread();
			float ConeHalfAngle = FMath::DegreesToRadians(((type == EFireType::FTE_Primary) ? weapon->Data.PrimarySpread : weapon->Data.SecondarySpread) * 0.5f);

			const FVector ShootDir = WeaponRandomStream.VRandCone(rotation.Vector(), ConeHalfAngle, ConeHalfAngle);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			//(type == EFireType::FTE_Primary) ? weapon->Data.PrimaryProjectileClass : weapon->Data.SecondaryProjectileClass
			if (type == EFireType::FTE_Primary && weapon->Data.PrimaryProjectileClass != NULL)
			{
				ABaseProjectile* proj = weapon->GetWorld()->SpawnActor<ABaseProjectile>(weapon->Data.PrimaryProjectileClass, location, ShootDir.Rotation(), ActorSpawnParams);
				if (proj != nullptr && proj->IsValidLowLevel())
				{
					proj->SetOwner(weapon->WeaponOwner);
					proj->WeaponOwner = weapon->WeaponOwner;
					proj->Damage = (type == EFireType::FTE_Primary) ? weapon->Data.PrimaryDamage : weapon->Data.SecondaryDamage;;
					proj->DamageType = (type == EFireType::FTE_Primary) ? weapon->Data.PrimaryDamageType : weapon->Data.SecondaryDamageType;;
				}
			}
			else if (type == EFireType::FTE_Secondary && weapon->Data.SecondaryProjectileClass != NULL)
			{
				ABaseProjectile* proj = weapon->GetWorld()->SpawnActor<ABaseProjectile>(weapon->Data.SecondaryProjectileClass, location, ShootDir.Rotation(), ActorSpawnParams);
				if (proj != nullptr && proj->IsValidLowLevel())
				{
					proj->SetOwner(weapon->WeaponOwner);
					proj->WeaponOwner = weapon->WeaponOwner;
					proj->Damage = (type == EFireType::FTE_Primary) ? weapon->Data.PrimaryDamage : weapon->Data.SecondaryDamage;;
					proj->DamageType = (type == EFireType::FTE_Primary) ? weapon->Data.PrimaryDamageType : weapon->Data.SecondaryDamageType;;
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Error Spawing projectile. Projectile class is NULL"));
			}

			CurrentFiringSpread = FMath::Min(((type == EFireType::FTE_Primary) ? weapon->Data.PrimarySpread : weapon->Data.SecondarySpread), CurrentFiringSpread + ((type == EFireType::FTE_Primary) ? weapon->Data.PrimarySpreadIncrement : weapon->Data.SecondarySpreadIncrement));
		}
		// spawn the projectile at the muzzle
		//weapon->GetWorld()->SpawnActor<ABaseProjectile>((type == EFireType::FTE_Primary) ? weapon->Data.PrimaryProjectileClass : weapon->Data.SecondaryProjectileClass, location, rotation, ActorSpawnParams);
	}
}