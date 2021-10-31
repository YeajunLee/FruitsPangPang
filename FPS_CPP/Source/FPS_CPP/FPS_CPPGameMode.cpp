// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPS_CPPGameMode.h"
#include "FPS_CPPCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFPS_CPPGameMode::AFPS_CPPGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
