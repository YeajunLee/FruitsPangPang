// Copyright Epic Games, Inc. All Rights Reserved.


#include "AIClientGameModeBase.h"

AAIClientGameModeBase::AAIClientGameModeBase()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Character/BP_MyCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}