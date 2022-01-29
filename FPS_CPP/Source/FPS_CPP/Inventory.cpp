// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"
#include "MainWidget.h"
#include "InventorySlotWidget.h"

// Sets default values
AInventory::AInventory()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInventory::BeginPlay()
{
	Super::BeginPlay();
	
	mSlots.Empty(mAmountOfSlots);	//Empty의 인자를 넣으면 Vector의 Reserve를 생각하면 될 것 같다. 
	for (int i = 0; i < mAmountOfSlots; ++i)
		mSlots.Add(FInventorySlot());

	if (mMainWidget == nullptr)
	{
		mMainWidget = CreateWidget<UMainWidget>(GetWorld(), mMakerMainWidget);
		if (mMainWidget != nullptr)
		{
			//... Do Something
			mMainWidget->mInventory = this;
			mMainWidget->minventorySlot->inventoryRef = this;
			mMainWidget->minventorySlot->mIndex = 0;
			mMainWidget->minventorySlot->Update();
			mMainWidget->AddToViewport();//Nativecontruct 호출 시점임.
			mMainWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

// Called every frame
void AInventory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInventory::AddItem(const FItemInfo& item, const int& amount)
{
	auto& slot = mSlots[item.IndexOfHotKeySlot];
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
	FString::Printf(TEXT("num: %d,amount %d"),item.IndexOfHotKeySlot,amount));
	if (slot.ItemClass.ItemCode == item.ItemCode)
	{
		slot.Amount += amount;
	}
	else {
		slot.ItemClass = item;
		slot.Amount = amount;
	}
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
	FString::Printf(TEXT("Slot Amount: %d"), slot.Amount));

	//mMainWidget->minventorySlot[item.IndexOfHotKeySlot]->Update(); <- 이런식으로 바뀔 예정
	mMainWidget->minventorySlot->Update();
}

void AInventory::UpdateInventorySlot(const FItemInfo& item, const int& amount)
{
	auto& slot = mSlots[item.IndexOfHotKeySlot];
	if (slot.ItemClass.ItemCode == item.ItemCode)
	{
		slot.Amount = amount;
	}
	else {
		slot.ItemClass = item;
		slot.Amount = amount;
	}
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
		FString::Printf(TEXT("Slot Amount: %d"), slot.Amount));

	//mMainWidget->minventorySlot[item.IndexOfHotKeySlot]->Update(); <- 이런식으로 바뀔 예정
	mMainWidget->minventorySlot->Update();
}

void AInventory::GetItemInfoAtSlotIndex(const int& index, __out bool& isempty, __out FItemInfo& iteminfo, __out int& amount)
{
	isempty = !IsSlotValid(index);
	if (!isempty)
	{
		iteminfo = mSlots[index].ItemClass;
		amount = mSlots[index].Amount;

		return;
	}

}

void AInventory::RemoveItemAtSlotIndex(const int& index, const int& amount)
{
	if (amount <= 0 || !IsSlotValid(index)) return;

	if (mSlots[index].Amount > amount )
	{
		mSlots[index].Amount -= amount;
		//mMainWidget->minventorySlot[item.IndexOfHotKeySlot]->Update(); <- 이런식으로 바뀔 예정
		mMainWidget->minventorySlot->Update();
		return;
	}

	mSlots[index].Amount = 0;
	mSlots[index].ItemClass = FItemInfo();
	mMainWidget->minventorySlot->Update();

}

bool AInventory::IsSlotValid(const int& index)
{
	
	if (mSlots[index].Amount <= 0) return false;
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
		FString::Printf(TEXT("ItemCode %d "), mSlots[index].ItemClass.ItemCode));
	if (mSlots[index].ItemClass.ItemCode < 1) return false;


	return true;
}

const FText AInventory::ItemCodeToItemName(const int& itemCode)
{
	switch (itemCode)
	{
	case 1:
		return FText::FromString(FString("Tomato"));
		break;
	case 2:
		return FText::FromString(FString("Quiui"));
		break;
	default:
		break;
	}
	return FText();
}