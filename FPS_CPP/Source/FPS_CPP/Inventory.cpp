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

}

void AInventory::UpdateInventorySlot(const FItemInfo& item, const int& amount)
{
	auto& slot = mSlots[item.IndexOfHotKeySlot];
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
		FString::Printf(TEXT("num: %d,amount %d"), item.IndexOfHotKeySlot, amount));
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

}

void AInventory::GetItemInfoAtSlotIndex(const int& index, __out bool& isempty, __out FItemInfo& iteminfo, __out int& amount)
{
	isempty = mSlots.IsValidIndex(index);
	if (!isempty)
	{
		iteminfo = mSlots[index].ItemClass;
		amount = mSlots[index].Amount;
		return;
	}

}