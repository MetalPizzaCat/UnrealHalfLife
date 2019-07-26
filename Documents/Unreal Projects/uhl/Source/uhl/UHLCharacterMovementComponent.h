// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UHLCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class UHL_API UUHLCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
		//UFUNCTION(BlueprintCallable, Category = "Pawn|Components|CharacterMovement")
		virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)override;
};
