// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include "Network.h"
#include "MyCharacter.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"




void ULoginWidget::NativePreConstruct()
{

	FInputModeUIOnly gamemode;
	auto controller = GetWorld()->GetFirstPlayerController();
	if (nullptr != controller)
	{
		controller->SetInputMode(gamemode);
		controller->SetShowMouseCursor(true);
	}
	LoginButton->OnClicked.AddDynamic(this, &ULoginWidget::TryLogin);
}

void ULoginWidget::NativeDestruct()
{
}

void ULoginWidget::TryLogin()
{
	if (nullptr != Network::GetNetwork()->mMyCharacter)
		send_login_packet(Network::GetNetwork()->mMyCharacter->l_socket, 0);
}
