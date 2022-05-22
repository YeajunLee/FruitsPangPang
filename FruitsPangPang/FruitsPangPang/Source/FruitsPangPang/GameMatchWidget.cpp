// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMatchWidget.h"
#include "Network.h"
#include "MyCharacter.h"
#include "Components/Button.h"
#include <Kismet/GameplayStatics.h>


void UGameMatchWidget::NativePreConstruct()
{
	MatchforPlayer->OnClicked.AddDynamic(this, &UGameMatchWidget::TryMatchingPlayer);
	MatchforAI->OnClicked.AddDynamic(this, &UGameMatchWidget::TryMatchingAI);
}

void UGameMatchWidget::NativeDestruct()
{

}


void UGameMatchWidget::TryMatchingPlayer()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("FruitsPangPangMap_Player"));
}


void UGameMatchWidget::TryMatchingAI()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("FruitsPangPangMap_AI"));
}

