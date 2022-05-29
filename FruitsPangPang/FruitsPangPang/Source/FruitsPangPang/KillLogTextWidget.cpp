// Fill out your copyright notice in the Description page of Project Settings.


#include "KillLogTextWidget.h"
#include "Components/TextBlock.h"

void UKillLogTextWidget::NativePreConstruct()
{
//	mMatchWaitOverlay->SetVisibility(ESlateVisibility::Hidden);
//#define LOCTEXT_NAMESPACE "match"
//	MatchingText->SetText(FText::FromString("Matching..."));
//	CurrentMatchingPlayerCnt->SetText(FText::Format(LOCTEXT("match", "(0/{0})"), MAX_PLAYER_CONN));
//#undef LOCTEXT_NAMESPACE
}

void UKillLogTextWidget::NativeDestruct()
{

}


void UKillLogTextWidget::NativeTick(const FGeometry& my, float deltatime)
{
	LifeTime -= deltatime;
	if (LifeTime < 0)
		RemoveFromParent();
}


void UKillLogTextWidget::MakeKillLog(const FString& attacker, const FString& victim)
{
	mAttacker->SetText(FText::FromString(attacker));
	mVictim->SetText(FText::FromString(victim));
}