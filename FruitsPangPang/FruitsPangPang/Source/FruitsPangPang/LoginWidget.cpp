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
	SignUpButton->OnClicked.AddDynamic(this, &ULoginWidget::TrySignUp);
}

void ULoginWidget::NativeDestruct()
{
}

void ULoginWidget::TryLogin()
{
	const char* tmpid = TCHAR_TO_ANSI(*id->GetText().ToString());
	const char* tmppass = TCHAR_TO_ANSI(*Password->GetText().ToString());
	if (nullptr != Network::GetNetwork()->mMyCharacter)
		send_login_lobby_packet(Network::GetNetwork()->mMyCharacter->l_socket, tmpid, tmppass);
}

void ULoginWidget::TrySignUp()
{
	const char* tmpid = TCHAR_TO_ANSI(*id->GetText().ToString());
	const char* tmppass = TCHAR_TO_ANSI(*Password->GetText().ToString());
	if (nullptr != Network::GetNetwork()->mMyCharacter)
		send_signup_packet(Network::GetNetwork()->mMyCharacter->l_socket, tmpid, tmppass);
}
