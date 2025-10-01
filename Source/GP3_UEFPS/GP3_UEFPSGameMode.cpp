// Copyright Epic Games, Inc. All Rights Reserved.

#include "GP3_UEFPSGameMode.h"
#include "GP3_UEFPSCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGP3_UEFPSGameMode::AGP3_UEFPSGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
