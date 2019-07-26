// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "uhlGameMode.h"
#include "uhlHUD.h"
#include "uhlCharacter.h"
#include "UObject/ConstructorHelpers.h"

AuhlGameMode::AuhlGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AuhlHUD::StaticClass();
}
