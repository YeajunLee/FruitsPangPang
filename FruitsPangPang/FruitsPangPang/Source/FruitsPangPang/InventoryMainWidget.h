// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryMainWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UInventoryMainWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* CanvasPanel;


	//UPROPERTY(meta = (BindWidget))
	TArray<class UInventorySlotWidget*> minventorySlot;

	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* InventoryBar;

	class AInventory* mInventory;
	
};