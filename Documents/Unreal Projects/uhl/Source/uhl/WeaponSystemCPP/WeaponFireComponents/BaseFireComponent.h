// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
//#include "WeaponSystemCPP/WeaponBase.h"
#include "BaseFireComponent.generated.h"

class AWeaponBase;

UENUM(BlueprintType)
enum class EFireType :uint8
{
	FTE_Primary UMETA(DisplayName="Primary"),
	FTE_Secondary UMETA(DisplayName = "Secondary")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class UHL_API UBaseFireComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBaseFireComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(/*BlueprintNativeEvent, */BlueprintCallable, Category = "Fire")
		virtual void Fire(EFireType type, FVector location, FRotator rotation, AWeaponBase* weapon);
};
