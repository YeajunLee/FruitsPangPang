// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPS_CPP_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* CanvasPanel;


	UPROPERTY(meta = (BindWidget))
	class UInventorySlotWidget* minventorySlot;

	class AInventory* mInventory;

};
