// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameMatchWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UGameMatchWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
		class UButton* MatchforAI;
	UPROPERTY(meta = (BindWidget))
		class UButton* MatchforPlayer;
	UPROPERTY(meta = (BindWidget))
		class UOverlay* mMatchWaitOverlay;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MatchingText;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CurrentMatchingPlayerCnt;


	UFUNCTION()
		void TryMatchingPlayer();
	UFUNCTION()
		void TryMatchingAI();

	void UpdatePlayerCntText(const int& cnt);
};
