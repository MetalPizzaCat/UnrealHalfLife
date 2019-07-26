// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CI_Interact.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCI_Interact : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UHL_API ICI_Interact
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interactions")
	void BeUsed(AActor*user);

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};
