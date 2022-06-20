// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoreWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UStoreWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativePreConstruct() override;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CashText;
	UPROPERTY(meta = (BindWidget))
		class UButton* QuitButton;

	UFUNCTION()
		void ClickQuit();

	class UMainWidget* mMainWidget;
};
