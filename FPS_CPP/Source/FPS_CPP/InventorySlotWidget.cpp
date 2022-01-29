// Fill out your copyright notice in the Description page of Project Settings.



#include "InventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Inventory.h"

void UInventorySlotWidget::NativePreConstruct()
{
	
}


void UInventorySlotWidget::NativeDestruct()
{

}


void UInventorySlotWidget::Update()
{
	

	bool localisEmpty;
	FItemInfo iteminfo;
	int amount;
	inventoryRef->GetItemInfoAtSlotIndex(mIndex, localisEmpty, iteminfo, amount);
	if (localisEmpty)
	{
		SetBlank();
	}
	else {
#define LOCTEXT_NAMESPACE "Inventory"
		Amount->SetText(FText::Format(LOCTEXT("Inventory", "x{0}"), amount));
#undef LOCTEXT_NAMESPACE
		Amount->SetVisibility(ESlateVisibility::Visible);
		Icon->SetBrushFromTexture(iteminfo.Icon);
		Icon->SetVisibility(ESlateVisibility::Visible);

	}

}

void UInventorySlotWidget::SetBlank()
{

	Icon->SetVisibility(ESlateVisibility::Hidden);
	Amount->SetVisibility(ESlateVisibility::Hidden);
}