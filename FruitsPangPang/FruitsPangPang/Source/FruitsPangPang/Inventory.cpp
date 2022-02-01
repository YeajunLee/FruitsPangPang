// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"
#include "InventoryMainWidget.h"
#include "InventorySlotWidget.h"
#include "Components/HorizontalBox.h"

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

	mSlots.Empty(mAmountOfSlots);	//Empty�� ���ڸ� ������ Vector�� Reserve�� �����ϸ� �� �� ����. 
	for (int i = 0; i < mAmountOfSlots; ++i)
		mSlots.Add(FInventorySlot());

	if (mInventoryMainWidget == nullptr)
	{
		mInventoryMainWidget = CreateWidget<UInventoryMainWidget>(GetWorld(), mMakerInventoryMainWidget);
		if (mInventoryMainWidget != nullptr)
		{
			//... Do Something
			mInventoryMainWidget->mInventory = this;
			for (int i = 0; i < 5; ++i)
			{
				auto slot = CreateWidget<UInventorySlotWidget>(GetWorld(), mMakerInventorySlotWidget);
				slot->inventoryRef = this;
				slot->mIndex = i;
				slot->Update();
				mInventoryMainWidget->InventoryBar->AddChildToHorizontalBox(slot);
				mInventoryMainWidget->minventorySlot.Add(slot);
			}
			mInventoryMainWidget->AddToViewport();//Nativecontruct ȣ�� ������.
			mInventoryMainWidget->SetVisibility(ESlateVisibility::Visible);
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
	if (slot.ItemClass.ItemCode == item.ItemCode)
	{
		slot.Amount += amount;
	}
	else {
		slot.ItemClass = item;
		slot.Amount = amount;
	}

	mInventoryMainWidget->minventorySlot[item.IndexOfHotKeySlot]->Update(); //<- �̷������� �ٲ� ����
	//mInventoryMainWidget->minventorySlot->Update();
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

	mInventoryMainWidget->minventorySlot[item.IndexOfHotKeySlot]->Update();// <- �̷������� �ٲ� ����
	//mInventoryMainWidget->minventorySlot->Update();
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

	if (mSlots[index].Amount > amount)
	{
		mSlots[index].Amount -= amount;
		mInventoryMainWidget->minventorySlot[index]->Update(); //<- �̷������� �ٲ� ����
		//mInventoryMainWidget->minventorySlot->Update();
		return;
	}

	mSlots[index].Amount = 0;
	mSlots[index].ItemClass = FItemInfo();
	mInventoryMainWidget->minventorySlot[index]->Update(); //<- �̷������� �ٲ� ����
	//mInventoryMainWidget->minventorySlot->Update();

}

void AInventory::ClearInventory()
{
	for (auto& slot : mSlots)
	{
		slot.Amount = 0;
		slot.ItemClass = FItemInfo();
	}
	for (auto& slot : mInventoryMainWidget->minventorySlot)
	{
		slot->Update();
	}
}

bool AInventory::IsSlotValid(const int& index)
{

	if (mSlots[index].Amount <= 0) return false;
	if (mSlots[index].ItemClass.ItemCode < 1) return false;


	return true;
}

const FText AInventory::ItemCodeToItemName(const int& itemCode)
{
	FText res;
	switch (itemCode)
	{
	case 1:
		res = FText::FromString(FString("Tomato"));
		break;
	case 2:
		res = FText::FromString(FString("Kiwi"));
		break;
	case 3:
		res = FText::FromString(FString("Watermelon"));
	default:
		break;
	}

	return res;
}

UTexture2D* AInventory::ItemCodeToItemIcon(const int& itemCode)
{
	UTexture2D* res = nullptr;
	switch (itemCode)
	{
	case 1:
		res = LoadObject<UTexture2D>(NULL, TEXT("/Game/Assets/Fruits/tomato/Icon_tomato.Icon_tomato"), NULL, LOAD_None, NULL);
		break;
	case 2:
		res = LoadObject<UTexture2D>(NULL, TEXT("/Game/Assets/Fruits/Kiwi/Icon_kiwi.Icon_kiwi"), NULL, LOAD_None, NULL);
		break;
	case 3:
		res = LoadObject<UTexture2D>(NULL, TEXT("/Game/Assets/Fruits/Watermelon/Icon_watermelon.Icon_watermelon"), NULL, LOAD_None, NULL);
		break;
	default:
		break;
	}
	return res;
}