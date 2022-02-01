// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

/**
 *
 */
UCLASS()
class FRUITSPANGPANG_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
		class UBorder* Border;
	UPROPERTY(meta = (BindWidget))
		class UOverlay* Overlay;

	UPROPERTY(meta = (BindWidget))
		class UImage* Icon;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Amount;

	bool isEmpty;
	class AInventory* inventoryRef;

	int mIndex;

	void SetBlank();
public:
	void Update();
};
