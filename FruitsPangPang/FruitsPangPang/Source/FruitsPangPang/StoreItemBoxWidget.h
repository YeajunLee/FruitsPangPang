// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StoreItemBoxWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UStoreItemBoxWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativePreConstruct() override;

	UPROPERTY(meta = (BindWidget))
		class UImage* ItemImage;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ItemName;
	UPROPERTY(meta = (BindWidget))
		class UButton* BuyBotton;
	UPROPERTY(meta = (BindWidget))
		class UButton* EquipButton;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* PriceText;

	UFUNCTION()
		void ClickBuy_0();
	
};
