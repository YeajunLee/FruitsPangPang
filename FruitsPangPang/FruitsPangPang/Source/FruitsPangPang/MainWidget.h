// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* CanvasPanel;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* InventoryBar;

	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* MinimapBox;

	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* ScoreBox;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* RemainGameTimeText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Time")
		float fRemainTime;	//Game Remain Time

	UFUNCTION(BlueprintCallable)
		void UpdateCountDown(const FText& minute, const FText& second);
	UFUNCTION(BlueprintCallable)
		const float ReduceRemainTime(const float& deltatime);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Time")
		bool bActivate = false;
	TArray<class UInventorySlotWidget*> minventorySlot;
	class URespawnWindowWidget* mRespawnWindowWidget;

	class UScoreWidget* mScoreWidget;

	class AInventory* mInventory;

	class ABaseCharacter* mOwnerCharacter;
	void UpdateHpBar();
	void ShowRespawnWidget();
	void HideRespawnWidget();
};
