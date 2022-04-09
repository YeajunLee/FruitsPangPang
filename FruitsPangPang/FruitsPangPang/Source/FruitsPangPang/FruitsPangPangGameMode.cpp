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
		//���⿡ ���� ������, �÷��̸� ���� ��, Level�� Character�� ���ٸ� �ڵ����� �������.
		//�츮�� Level�� ��ġ�� �ϰų�, SpawnActor�� ��ġ���ٰŰ� �ƹ��͵����ٸ� �� �״�� �ƹ��͵� �ȶ���� ������ NULL�� �ٲ�.
		DefaultPawnClass = NULL;// PlayerPawnBPClass.Class;
		
	}
}
