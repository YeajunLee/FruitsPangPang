// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreWidget.h"
#include "Components/Button.h"
#include "MyCharacter.h"
#include "Network.h"
#include "MainWidget.h"



void UStoreWidget::NativePreConstruct()
{	
	QuitButton-> OnClicked.AddDynamic(this, &UStoreWidget::ClickQuit);
}

void UStoreWidget::ClickQuit()
{
	FInputModeUIOnly gamemode;
	auto controller = Network::GetNetwork()->mMyCharacter->GetWorld()->GetFirstPlayerController();
	controller->SetInputMode(gamemode);
	controller->SetShowMouseCursor(false);
	Network::GetNetwork()->mMyCharacter->mMainWidget->HideStoreWidget();
}

