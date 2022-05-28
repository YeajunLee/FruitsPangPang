// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMatchWidget.h"
#include "Network.h"
#include "MyCharacter.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include <Kismet/GameplayStatics.h>


void UGameMatchWidget::NativePreConstruct()
{
	MatchforPlayer->OnClicked.AddDynamic(this, &UGameMatchWidget::TryMatchingPlayer);
	MatchforAI->OnClicked.AddDynamic(this, &UGameMatchWidget::TryMatchingAI);

	mMatchWaitOverlay->SetVisibility(ESlateVisibility::Hidden);
#define LOCTEXT_NAMESPACE "match"
	MatchingText->SetText(FText::FromString("Matching..."));
	CurrentMatchingPlayerCnt->SetText(FText::Format(LOCTEXT("match","(0/{0})"), MAX_PLAYER_CONN));	
#undef LOCTEXT_NAMESPACE
}

void UGameMatchWidget::NativeDestruct()
{

}


void UGameMatchWidget::TryMatchingPlayer()
{
	send_match_request(Network::GetNetwork()->mMyCharacter->l_socket);
}


void UGameMatchWidget::TryMatchingAI()
{
	Network::GetNetwork()->mGameMode = 1;
	send_match_request(Network::GetNetwork()->mMyCharacter->l_socket, ACTIVE_AI_CNT);
}

void UGameMatchWidget::UpdatePlayerCntText(const int& cnt)
{
	mMatchWaitOverlay->SetVisibility(ESlateVisibility::Visible);
	MatchforPlayer->SetIsEnabled(false);
	MatchforAI->SetIsEnabled(false);
#define LOCTEXT_NAMESPACE "match"
	CurrentMatchingPlayerCnt->SetText(FText::Format(LOCTEXT("match", "({0}/{1})"),cnt,MAX_PLAYER_CONN));
#undef LOCTEXT_NAMESPACE
}