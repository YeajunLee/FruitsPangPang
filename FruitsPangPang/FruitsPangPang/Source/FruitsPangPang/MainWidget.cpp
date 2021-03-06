// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidget.h"
#include "Components/ProgressBar.h"
#include "MyCharacter.h"
#include "BaseCharacter.h"
#include "Network.h"
#include "RespawnWindowWidget.h"
#include "ScoreWidget.h"
#include "KillLogTextWidget.h"
#include "MiniMapWidget.h"
#include "StoreWidget.h"
#include "GameMatchWaitingWidget.h"
#include "ChatWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"




void UMainWidget::NativePreConstruct()
{
	HPBar->Percent = 1.0f;
	fRemainTime = GAMEPLAYTIME_MILLI / 1000;
	switch (WidgetType)
	{
	case -1:
		//UE_LOG(LogTemp, Error, TEXT("Widget Type Wrong !"));
		break;
	case 0: {
		{
			RemainGameTimeText->SetVisibility(ESlateVisibility::Hidden);
			W_MiniMap_0->SetVisibility(ESlateVisibility::Hidden);
			FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MStoreWidget.MStoreWidget_C'"));
			auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
			auto StoreWGT = CreateWidget<UStoreWidget>(GetWorld(), WidgetClass);
			W_Store = StoreWGT;
			auto SettupStoreBox = StoreBox->AddChildToHorizontalBox(StoreWGT);
			FSlateChildSize Rules;
			Rules.SizeRule = ESlateSizeRule::Fill;
			SettupStoreBox->SetSize(Rules);
			HideStoreWidget();
		}
		{
			FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MGameMatchWaitingWidget.MGameMatchWaitingWidget_C'"));
			auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
			auto GameMatchWaitingWGT = CreateWidget<UGameMatchWaitingWidget>(GetWorld(), WidgetClass);
			W_MatchWaiting = GameMatchWaitingWGT;
			MatchWaitingBox->AddChildToHorizontalBox(GameMatchWaitingWGT);
			HideMatchWaitingWidget();
		}
		{
			FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MChatWidget.MChatWidget_C'"));
			auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
			auto ChatWGT = CreateWidget<UChatWidget>(GetWorld(), WidgetClass);
			W_Chat = ChatWGT;
			ChatBox->AddChildToHorizontalBox(ChatWGT);
		}
		break;
	}
	case 1: {
		{
			RemainGameTimeText->SetVisibility(ESlateVisibility::Visible);
			W_MiniMap_0->SetVisibility(ESlateVisibility::Visible);
			FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MRespawnWindowWidget.MRespawnWindowWidget_C'"));
			auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
			auto RespawnWGT = CreateWidget<URespawnWindowWidget>(GetWorld(), WidgetClass);
			RespawnBox->AddChildToHorizontalBox(RespawnWGT);
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
		break;
	}
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
	mRespawnWindowWidget->bActivate = true;
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
	mRespawnWindowWidget->bActivate = false;
	mRespawnWindowWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UMainWidget::ShowStoreWidget()
{
	FInputModeUIOnly gamemode;
	auto controller = GetWorld()->GetFirstPlayerController();
	controller->SetInputMode(gamemode);
	controller->SetShowMouseCursor(true);
	W_Store->UpdateCash(Network::GetNetwork()->mMyCharacter->Cash);
	W_Store->SetVisibility(ESlateVisibility::Visible);
}

void UMainWidget::HideStoreWidget()
{
	FInputModeGameOnly gamemode;
	auto controller = GetWorld()->GetFirstPlayerController();
	if (nullptr != controller)
	{
		controller->SetInputMode(gamemode);
		controller->SetShowMouseCursor(false);
	}
	W_Store->SetVisibility(ESlateVisibility::Hidden);
}

void UMainWidget::ShowMatchWaitingWidget()
{
	W_MatchWaiting->SetVisibility(ESlateVisibility::Visible);
}

void UMainWidget::HideMatchWaitingWidget()
{
	W_MatchWaiting->SetVisibility(ESlateVisibility::Hidden);
}

void UMainWidget::UpdateCountDown(const FText& minute, const FText& second)
{
#define LOCTEXT_NAMESPACE "Game"
	RemainGameTimeText->SetText(FText::Format(LOCTEXT("Game", "{0}:{1}"), minute, second));
#undef LOCTEXT_NAMESPACE
}

const float UMainWidget::ReduceRemainTime(const float& deltatime)
{
	fRemainTime = FMath::Max<float>(fRemainTime - deltatime, 0.0f);
	return fRemainTime;
}

void UMainWidget::GameEndSoon()
{
	
	UGameplayStatics::SpawnSound2D(this, Network::GetNetwork()->mMyCharacter->TickTockBGM);
	Network::GetNetwork()->mMyCharacter->SpawnedInGameBGM->FadeOut(10.0f, 0.0f);
	RemainGameTimeText->SetColorAndOpacity(FSlateColor(FLinearColor(1, 0, 0)));
}


void UMainWidget::UpdateKillLog(const FString& Attacker, const FString& Victim)
{
	FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MKillLogTextWidget.MKillLogTextWidget_C'"));
	auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
	if (nullptr == WidgetClass)
	{
		//UE_LOG(LogTemp, Warning, TEXT("MainWidget Source is invalid !! check '/Game/Widget/MKillLogTextWidget.MKillLogTextWidget_C'"));
		return;
	}
	auto KillLogTextWGT = CreateWidget<UKillLogTextWidget>(GetWorld(), WidgetClass);
	KillLogTextWGT->MakeKillLog(Attacker, Victim);
	KillLogBox->AddChild(KillLogTextWGT);
}