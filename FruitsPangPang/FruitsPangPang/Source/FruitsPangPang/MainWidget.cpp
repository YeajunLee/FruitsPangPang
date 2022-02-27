// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"
#include "Components/ProgressBar.h"
#include "MyCharacter.h"
#include "Network.h"


void UMainWidget::NativePreConstruct()
{
	HPBar->Percent = 1.0f;
}


void UMainWidget::NativeDestruct()
{

}


void UMainWidget::UpdateHpBar()
{
	HPBar->SetPercent(static_cast<float>(mCharacter->hp) / static_cast<float>(PLAYER_HP));
}