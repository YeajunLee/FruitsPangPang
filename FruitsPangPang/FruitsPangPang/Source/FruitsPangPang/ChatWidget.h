// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UChatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* EditableChatBox;


	UPROPERTY(meta = (BindWidget))
		class UScrollBox* ChatLogBox;

	void ControlChat();
	void UpdateChat(const FString& name, FText&& msg);
	void UpdateChat(const FString& name, FText& msg);
	UFUNCTION()
		void ChatBoxCommited(const FText& Text, ETextCommit::Type CommitType);
	UFUNCTION()
		void ChatBoxChanged(const FText& Text);

};
