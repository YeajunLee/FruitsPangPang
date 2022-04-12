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
		//여기에 값이 있으면, 플레이를 했을 때, Level에 Character가 없다면 자동으로 만들어줌.
		//우리는 Level에 배치를 하거나, SpawnActor로 배치해줄거고 아무것도없다면 말 그대로 아무것도 안띄워도 상관없어서 NULL로 바꿈.
		DefaultPawnClass = NULL;// PlayerPawnBPClass.Class;
		
	}
}
