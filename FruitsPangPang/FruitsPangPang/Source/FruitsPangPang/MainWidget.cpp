// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"
#include "Components/ProgressBar.h"
#include "MyCharacter.h"
#include "BaseCharacter.h"
#include "Network.h"
#include "RespawnWindowWidget.h"
#include "ScoreWidget.h"
#include "Components/HorizontalBox.h"


void UMainWidget::NativePreConstruct()
{
	HPBar->Percent = 1.0f;

	{
		FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MRespawnWindowWidget.MRespawnWindowWidget_C'"));
		auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
		auto RespawnWGT = CreateWidget<URespawnWindowWidget>(GetWorld(), WidgetClass);
		MinimapBox->AddChildToHorizontalBox(RespawnWGT);
		mRespawnWindowWidget = RespawnWGT;
		HideRespawnWidget();
	}
	{
		FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MScoreWidget.MScoreWidget_C'"));
		auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
		auto ScoreWGT = CreateWidget<UScoreWidget>(GetWorld(), WidgetClass);
		ScoreBox->AddChildToHorizontalBox(ScoreWGT);
		mScoreWidget = ScoreWGT;
	}





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
	if (nullptr != controller)
	{
		controller->SetInputMode(gamemode);
		controller->SetShowMouseCursor(false);
	}
	mRespawnWindowWidget->Activate = false;
	mRespawnWindowWidget->SetVisibility(ESlateVisibility::Hidden);
}