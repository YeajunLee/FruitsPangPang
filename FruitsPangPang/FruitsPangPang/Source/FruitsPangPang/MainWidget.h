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
		class UHorizontalBox* RespawnBox;

	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* ScoreBox;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* RemainGameTimeText;

	UPROPERTY(meta = (BindWidget))
		class UScrollBox* KillLogBox;


	UPROPERTY(BlueprintReadWrite,meta = (BindWidget))
		class UMiniMapWidget* W_MiniMap_0;

	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* StoreBox;

	class UStoreWidget* W_Store;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Time")
		float fRemainTime;	//Game Remain Time

	UFUNCTION(BlueprintCallable)
		void UpdateCountDown(const FText& minute, const FText& second);
	UFUNCTION(BlueprintCallable)
		const float ReduceRemainTime(const float& deltatime);

	UFUNCTION(BlueprintCallable)
		void GameEndSoon();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Time")
		int WidgetType = -1;	//  -1 - Invalid, 0 - Lobby, 1 - Ingame
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
	void ShowStoreWidget();
	void HideStoreWidget();
	void UpdateKillLog(const FString& Attacker,const FString& Victim);
};
