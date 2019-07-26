// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "WeaponSystemCPP/WeaponBase.h"
#include "BaseProjectile.generated.h"

UCLASS()
class UHL_API ABaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, SaveGame, Category = Projectile)
		class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, SaveGame, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		class UProjectileMovementComponent* ProjectileMovement;
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	AActor* WeaponOwner;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
		float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
		TSubclassOf<UDamageType> DamageType;
	/*UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
		AWeaponBase* FiredWeapon;*/
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
