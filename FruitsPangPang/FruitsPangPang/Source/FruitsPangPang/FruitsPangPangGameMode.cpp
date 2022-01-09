// Copyright Epic Games, Inc. All Rights Reserved.

#include "FruitsPangPangGameMode.h"
#include "FruitsPangPangCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFruitsPangPangGameMode::AFruitsPangPangGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Character/BP_MyCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
