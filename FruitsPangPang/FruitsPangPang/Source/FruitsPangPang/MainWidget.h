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

	TArray<class UInventorySlotWidget*> minventorySlot;

	class AInventory* mInventory;

	class AMyCharacter* mCharacter;
	void UpdateHpBar();
};
