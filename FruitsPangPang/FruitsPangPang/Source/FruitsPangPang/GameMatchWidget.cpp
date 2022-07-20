// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMatchWidget.h"
#include "Network.h"
#include "MyCharacter.h"
#include "MainWidget.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include <Kismet/GameplayStatics.h>


void UGameMatchWidget::NativePreConstruct()
{
	MatchforPlayer->OnClicked.AddDynamic(this, &UGameMatchWidget::TryMatchingPlayer);
	if(Network::GetNetwork()->bIsAlreadyMatching)
		MatchforPlayer->SetIsEnabled(false);
}

void UGameMatchWidget::NativeDestruct()
{

}


void UGameMatchWidget::TryMatchingPlayer()
{
	if (!Network::GetNetwork()->bIsAlreadyMatching)
	{
		Network::GetNetwork()->bIsAlreadyMatching = true;
		if(Network::GetNetwork()->mMyCharacter->mMainWidget)
			Network::GetNetwork()->mMyCharacter->mMainWidget->ShowMatchWaitingWidget();
		send_match_request(Network::GetNetwork()->mMyCharacter->l_socket);
		FInputModeGameOnly gamemode;
		auto controller = GetWorld()->GetFirstPlayerController();
		if (nullptr != controller)
		{
			controller->SetInputMode(gamemode);
			controller->SetShowMouseCursor(false);
		}
		this->RemoveFromParent();
	}
}