// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMatchWidget.h"
#include "Network.h"
#include "MyCharacter.h"
#include "Components/Button.h"
#include <Kismet/GameplayStatics.h>


void UGameMatchWidget::NativePreConstruct()
{
	MatchButton->OnClicked.AddDynamic(this, &UGameMatchWidget::TryMatching);
}

void UGameMatchWidget::NativeDestruct()
{

}


void UGameMatchWidget::TryMatching()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("FruitsPangPangMap_Player"));
}