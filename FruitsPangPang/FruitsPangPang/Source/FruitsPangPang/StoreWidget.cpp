// Fill out your copyright notice in the Description page of Project Settings.


#include "StoreWidget.h"
#include "Components/Button.h"
#include "Components/WrapBox.h"
#include "MyCharacter.h"
#include "Network.h"
#include "MainWidget.h"
#include "StoreItemBoxWidget.h"
#include "Components/TextBlock.h"
#include "Engine/DataTable.h"
#include "Item.h"


void UStoreWidget::NativePreConstruct()
{	
	QuitButton-> OnClicked.AddDynamic(this, &UStoreWidget::ClickQuit);

	UDataTable* StoreItemList = nullptr;
	FName Path("DataTable'/Game/Widget/Assets/Shop.Shop'");
	StoreItemList = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), NULL, *Path.ToString()));
	if (nullptr != StoreItemList)
	{
		TArray<FStoreItemInfo*> row;
		FString context;
		StoreItemList->GetAllRows(context, row);
		for (auto p : row)
		{
			InsertItem(p->ItemCode, p->Name, p->Price);
		}
	}
}

void UStoreWidget::ClickQuit()
{
	FInputModeUIOnly gamemode;
	auto controller = Network::GetNetwork()->mMyCharacter->GetWorld()->GetFirstPlayerController();
	controller->SetInputMode(gamemode);
	controller->SetShowMouseCursor(false);
	Network::GetNetwork()->mMyCharacter->mMainWidget->HideStoreWidget();
}

void UStoreWidget::InsertItem(const int& itemCode, const FText& itemName,const int& price)
{
	FSoftClassPath WidgetSource(TEXT("WidgetBlueprint'/Game/Widget/MStoreItemBoxWidget.MStoreItemBoxWidget_C'"));
	auto WidgetClass = WidgetSource.TryLoadClass<UUserWidget>();
	auto itemWGT = CreateWidget<UStoreItemBoxWidget>(GetWorld(), WidgetClass);
	if (nullptr == itemWGT)
	{
		UE_LOG(LogTemp, Warning, TEXT("StoreItemBoxWidget Source is invalid !! check '/Game/Widget/MStoreItemBoxWidget.MStoreItemBoxWidget_C'"));
	}
	else {
		itemWGT->InitializeItem(itemCode, itemName, price);

		StuffWrapBox->AddChildToWrapBox(itemWGT);
		items.Add(itemWGT);
	}

}
void UStoreWidget::UpdateItemSlotStatus(const int& itemcode)
{

	auto item = items.FindByPredicate([&itemcode](UStoreItemBoxWidget* wgt) {
		return wgt->mitemcode == itemcode;
	});
	if (nullptr != item)
	{
		(*item)->BuyButton->SetIsEnabled(false);
		(*item)->EquipButton->SetIsEnabled(true);
	}
	
}

void UStoreWidget::UpdateCash(const int& Cash)
{
#define LOCTEXT_NAMESPACE "cash"
	CashText->SetText(FText::Format(LOCTEXT("cash", "{0}"), Cash));
#undef LOCTEXT_NAMESPACE
}