// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"
#include "Components/ProgressBar.h"
#include "MyCharacter.h"
#include "BaseCharacter.h"
#include "Network.h"
#include "RespawnWindowWidget.h"
#include "Components/HorizontalBox.h"


void UMainWidget::NativePreConstruct()
{
	HPBar->Percent = 1.0f;


	FSoftClassPath my2(TEXT("WidgetBlueprint'/Game/Widget/MRespawnWindowWidget.MRespawnWindowWidget_C'"));
	auto p2 = my2.TryLoadClass<UUserWidget>();
	auto wi2 = CreateWidget<URespawnWindowWidget>(GetWorld(), p2);
	MinimapBox->AddChildToHorizontalBox(wi2);
	mRespawnWindowWidget = wi2;
	HideRespawnWidget();
}


void UMainWidget::NativeDestruct()
{

}


void UMainWidget::UpdateHpBar()
{
	HPBar->SetPercent(static_cast<float>(mOwnerCharacter->hp) / static_cast<float>(PLAYER_HP));
}

void UMainWidget::ShowRespawnWidget()
{
	
	FInputModeUIOnly gamemode;
	auto controller = GetWorld()->GetFirstPlayerController();
	controller->SetInputMode(gamemode);
	controller->SetShowMouseCursor(true);
	mRespawnWindowWidget->SetVisibility(ESlateVisibility::Visible);
	mRespawnWindowWidget->RemainTime = mRespawnWindowWidget->RespawnTime;
	mRespawnWindowWidget->Activate = true;
}

void UMainWidget::HideRespawnWidget()
{
	FInputModeGameOnly gamemode;
	auto controller = GetWorld()->GetFirstPlayerController();
	controller->SetInputMode(gamemode);
	controller->SetShowMouseCursor(false);
	mRespawnWindowWidget->Activate = false;
	mRespawnWindowWidget->SetVisibility(ESlateVisibility::Hidden);
}