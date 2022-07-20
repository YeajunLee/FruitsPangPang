// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMatchWaitingWidget.h"
#include "Network.h"
#include "MyCharacter.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include <Kismet/GameplayStatics.h>


void UGameMatchWaitingWidget::NativePreConstruct()
{

#define LOCTEXT_NAMESPACE "match"
	MatchingText->SetText(FText::FromString("Matching..."));
	CurrentMatchingPlayerCnt->SetText(FText::Format(LOCTEXT("match", "(0/{0})"), MAX_PLAYER_CONN));
#undef LOCTEXT_NAMESPACE
}

void UGameMatchWaitingWidget::NativeDestruct()
{

}




void UGameMatchWaitingWidget::UpdatePlayerCntText(const int& cnt)
{
	mMatchWaitOverlay->SetVisibility(ESlateVisibility::Visible);
#define LOCTEXT_NAMESPACE "match"
	CurrentMatchingPlayerCnt->SetText(FText::Format(LOCTEXT("match", "({0}/{1})"), cnt, MAX_PLAYER_CONN));
#undef LOCTEXT_NAMESPACE
}
