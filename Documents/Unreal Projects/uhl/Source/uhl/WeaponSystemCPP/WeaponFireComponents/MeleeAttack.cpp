// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAttack.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Physics/PhysicsMaterial.h"
#include "Engine.h"

void UMeleeAttack::Fire(EFireType type, FVector location, FRotator rotation, AWeaponBase* weapon)
{

	if (weapon->GetWorld() != NULL)
	{
		FVector end = location + (rotation.Vector() * 200);

		FHitResult hitResult;
		FCollisionQueryParams queryParams = FCollisionQueryParams::DefaultQueryParam;
		if (weapon->WeaponOwner != NULL && weapon->WeaponOwner->IsValidLowLevel()) { queryParams.AddIgnoredActor(weapon->WeaponOwner); }

		weapon->GetWorld()->LineTraceSingleByChannel(hitResult, location, end, ECollisionChannel::ECC_Camera, queryParams);
		if (hitResult.Actor.IsValid())
		{
			FPointDamageEvent PointDmg;
			PointDmg.DamageTypeClass = (type == EFireType::FTE_Primary) ? weapon->Data.PrimaryDamageType : weapon->Data.SecondaryDamageType;
			PointDmg.HitInfo = hitResult;
			PointDmg.ShotDirection = rotation.Vector();
			PointDmg.Damage = (type == EFireType::FTE_Primary) ? weapon->Data.PrimaryDamage : weapon->Data.SecondaryDamage;


			UGameplayStatics::ApplyPointDamage
			(
				hitResult.GetActor(),
				(type == EFireType::FTE_Primary) ? weapon->Data.PrimaryDamage : weapon->Data.SecondaryDamage,
				hitResult.ImpactPoint,
				hitResult,
				Cast<APawn>(weapon->WeaponOwner)->GetController(),
				weapon->WeaponOwner,
				(type == EFireType::FTE_Primary) ? weapon->Data.PrimaryDamageType : weapon->Data.SecondaryDamageType
			);
			if (hitResult.GetComponent() != nullptr)
			{
				hitResult.GetComponent()->AddImpulseAtLocation(
					(type == EFireType::FTE_Primary) ? weapon->Data.PrimaryDamageType.GetDefaultObject()->DamageImpulse * rotation.Vector() : weapon->Data.SecondaryDamageType.GetDefaultObject()->DamageImpulse * rotation.Vector(),
					hitResult.Location
				);

				if (hitResult.GetComponent()->GetMaterial(0) != nullptr)
				{
					if (hitResult.GetComponent()->GetMaterial(0)->GetPhysicalMaterial() != nullptr)
					{
						

						if (Cast<UPhysicsMaterial>(hitResult.GetComponent()->GetMaterial(0)->GetPhysicalMaterial()) != nullptr)
						{
							if (Cast<UPhysicsMaterial>(hitResult.GetComponent()->GetMaterial(0)->GetPhysicalMaterial())->BulletHitSound != nullptr)
							{
								UGameplayStatics::PlaySoundAtLocation(weapon->GetWorld(), Cast<UPhysicsMaterial>(hitResult.GetComponent()->GetMaterial(0)->GetPhysicalMaterial())->BulletHitSound, hitResult.Location);
								
							}

							if (Cast<UPhysicsMaterial>(hitResult.GetComponent()->GetMaterial(0)->GetPhysicalMaterial())->BulletImpactParticleEffect != nullptr)
							{
								UGameplayStatics::SpawnEmitterAtLocation(weapon->GetWorld(), Cast<UPhysicsMaterial>(hitResult.GetComponent()->GetMaterial(0)->GetPhysicalMaterial())->BulletImpactParticleEffect, hitResult.Location, (hitResult.Location - location).GetSafeNormal().Rotation());
							}
						}
					}
				}
			}
			//hitResult.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, Cast<APawn>(weapon->WeaponOwner)->GetController(), weapon->WeaponOwner);

			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, hitResult.Actor->GetFName().ToString());
			/*GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, (hitResult.Actor.Get() == hitResult.Actor) ? "True" : "False");*/
		}
		if (hitResult.Location != FVector::ZeroVector)
		{
			//DrawDebugLine(this->GetWorld(), location, hitResult.Location, FColor::Green, false, 5.f, 0, 1.f);
		}
		else
		{
			//DrawDebugLine(this->GetWorld(), location, end, FColor::Red, false, 5.f, 0, 1.f);
		}

		// spawn the projectile at the muzzle
		//weapon->GetWorld()->SpawnActor<ABaseProjectile>((type == EFireType::FTE_Primary) ? weapon->Data.PrimaryProjectileClass : weapon->Data.SecondaryProjectileClass, location, rotation, ActorSpawnParams);
	}
}