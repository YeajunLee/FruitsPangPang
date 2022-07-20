// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatTextWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UChatTextWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* mText;

	void MakeChatText(const FString& name, const FString& msg);

	FString WrapTextTooLongWord(FText& str, float wrapsize);
};
