// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "KillLogTextWidget.h"
#include "MyCharacter.h"
#include "ChatTextWidget.h"
#include "Network.h"
#include "Kismet/GameplayStatics.h"


void UChatWidget::NativePreConstruct()
{
	EditableChatBox->OnTextCommitted.AddDynamic(this, &UChatWidget::ChatBoxCommited);
	EditableChatBox->OnTextChanged.AddDynamic(this, &UChatWidget::ChatBoxChanged);
}

void UChatWidget::NativeDestruct()
{

}

void UChatWidget::ChatBoxCommited(const FText& Text, ETextCommit::Type CommitType)
{
	switch (CommitType)
	{
	case ETextCommit::Type::OnEnter: {
		FInputModeGameOnly gamemode;
		auto controller = GetWorld()->GetFirstPlayerController();
		if (nullptr != controller)
		{
			controller->SetInputMode(gamemode);
		}
		send_chat_packet(Network::GetNetwork()->mMyCharacter->l_socket, *FText::TrimPrecedingAndTrailing(Text).ToString());
		EditableChatBox->SetText(FText());
		break;
	}
	default: {
		FInputModeGameOnly gamemode;
		auto controller = GetWorld()->GetFirstPlayerController();
		if (nullptr != controller)
		{
			controller->SetInputMode(gamemode);
		}
	}
	}
}

void UChatWidget::ChatBoxChanged(const FText& Text)
{
	if (Text.ToString().Len() > 30)
		EditableChatBox->SetText(FText::FromString(Text.ToString().Mid(0, 30)));
}


void UChatWidget::ControlChat()
{
	if (!EditableChatBox->HasKeyboardFocus())
	{
		EditableChatBox->SetKeyboardFocus();
	}
}

void UChatWidget::UpdateChat(const FString& name, FText&& msg)
{
	FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MChatTextWidget.MChatTextWidget_C'"));
	auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
	if (nullptr == WidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UChatWidget Source is invalid !! check WidgetBlueprint'/Game/Widget/MChatTextWidget.MChatTextWidget'"));
		return;
	}
	auto ChatTextWGT = CreateWidget<UChatTextWidget>(GetWorld(), WidgetClass);
	ChatTextWGT->MakeChatText(name, msg.ToString());
	ChatLogBox->AddChild(ChatTextWGT);
	ChatLogBox->ScrollToEnd();
	if (ChatLogBox->GetAllChildren().Num() > 10)
		ChatLogBox->GetAllChildren()[0]->RemoveFromParent();
}

void UChatWidget::UpdateChat(const FString& name, FText& msg)
{
	FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MChatTextWidget.MChatTextWidget_C'"));
	auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
	if (nullptr == WidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UChatWidget Source is invalid !! check WidgetBlueprint'/Game/Widget/MChatTextWidget.MChatTextWidget'"));
		return;
	}
	auto ChatTextWGT = CreateWidget<UChatTextWidget>(GetWorld(), WidgetClass);
	ChatTextWGT->MakeChatText(name, msg.ToString());
	ChatLogBox->AddChild(ChatTextWGT);
	ChatLogBox->ScrollToEnd();
	if (ChatLogBox->GetAllChildren().Num() > 10)
		ChatLogBox->GetAllChildren()[0]->RemoveFromParent();
}