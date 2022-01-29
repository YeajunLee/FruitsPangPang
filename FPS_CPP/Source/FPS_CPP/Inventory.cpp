// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"
#include "MainWidget.h"
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

	if (mMainWidget == nullptr)
	{
		mMainWidget = CreateWidget<UMainWidget>(GetWorld(), mMakerMainWidget);
		if (mMainWidget != nullptr)
		{
			//... Do Something
			mMainWidget->mInventory = this;
			for (int i = 0; i < 5; ++i)
			{
				auto slot = CreateWidget<UInventorySlotWidget>(GetWorld(), mMakerInventorySlotWidget);
				slot->inventoryRef = this;
				slot->mIndex = 0;
				slot->Update();
				mMainWidget->InventoryBar->AddChildToHorizontalBox(slot);
				mMainWidget->minventorySlot.Add(slot);
			}
			mMainWidget->AddToViewport();//Nativecontruct ȣ�� ������.
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
	if (slot.ItemClass.ItemCode == item.ItemCode)
	{
		slot.Amount += amount;
	}
	else {
		slot.ItemClass = item;
		slot.Amount = amount;
	}

	mMainWidget->minventorySlot[item.IndexOfHotKeySlot]->Update(); //<- �̷������� �ٲ� ����
	//mMainWidget->minventorySlot->Update();
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

	mMainWidget->minventorySlot[item.IndexOfHotKeySlot]->Update();// <- �̷������� �ٲ� ����
	//mMainWidget->minventorySlot->Update();
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
		mMainWidget->minventorySlot[index]->Update(); //<- �̷������� �ٲ� ����
		//mMainWidget->minventorySlot->Update();
		return;
	}

	mSlots[index].Amount = 0;
	mSlots[index].ItemClass = FItemInfo(); 
	mMainWidget->minventorySlot[index]->Update(); //<- �̷������� �ٲ� ����
	//mMainWidget->minventorySlot->Update();

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
	FText res;
	switch (itemCode)
	{
	case 1:
		res = FText::FromString(FString("Tomato"));
		break;
	case 2:
		res = FText::FromString(FString("Quiui"));
		break;
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
		res = LoadObject<UTexture2D>(NULL, TEXT("/Game/Objects/Icon_Potion.Icon_Potion"), NULL, LOAD_None, NULL);
		break;
	case 2:
		break;
	default:
		break;
	}
	return res;
}