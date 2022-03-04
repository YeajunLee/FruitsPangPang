// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"

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

	//mMainWidget->minventorySlot[item.IndexOfHotKeySlot]->Update(); //<- 이런식으로 바뀔 예정
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

	//mMainWidget->minventorySlot[item.IndexOfHotKeySlot]->Update();// <- 이런식으로 바뀔 예정
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
		//mMainWidget->minventorySlot[index]->Update(); //<- 이런식으로 바뀔 예정
		return;
	}

	mSlots[index].Amount = 0;
	mSlots[index].ItemClass = FItemInfo();
	//mMainWidget->minventorySlot[index]->Update(); //<- 이런식으로 바뀔 예정

}

void AInventory::ClearInventory()
{
	for (auto& slot : mSlots)
	{
		slot.Amount = 0;
		slot.ItemClass = FItemInfo();
	}
	//for (auto& slot : mMainWidget->minventorySlot)
	//{
	//	slot->Update();
	//}
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
	case 4:
		res = FText::FromString(FString("Nut"));
	case 5:
		res = FText::FromString(FString("Durian"));
	default:
		break;
	}

	return res;
}

const FName AInventory::ItemCodeToItemBombPath(const int& itemCode)
{
	switch (itemCode)
	{
	case 1:
		return TEXT("Blueprint'/Game/Assets/Fruits/tomato/TomatoBomb.TomatoBomb_C'");
	case 2:
		return TEXT("Blueprint'/Game/Assets/Fruits/tomato/TomatoBomb.TomatoBomb_C'");
	case 3:
		return TEXT("Blueprint'/Game/Assets/Fruits/tomato/TomatoBomb.TomatoBomb_C'");
	case 4:
		return TEXT("Blueprint'/Game/Assets/Fruits/Nut/NutBomb.NutBomb_C'");
	case 5:
		return TEXT("Blueprint'/Game/Assets/Fruits/Durian/DurianBomb.DurianBomb_C'");
	default:
		return TEXT("None");
	}
}

const FName AInventory::ItemCodeToItemFruitPath(const int& itemCode)
{
	switch (itemCode)
	{
	case 1:
		return TEXT("Blueprint'/Game/Assets/Fruits/tomato/Fruit_Tomato.Fruit_Tomato_C'");
	case 2:
		return TEXT("Blueprint'/Game/Assets/Fruits/Kiwi/Fruit_Kiwi.Fruit_Kiwi_C'");
	case 3:
		return TEXT("Blueprint'/Game/Assets/Fruits/Watermelon/Fruit_Watermelon.Fruit_Watermelon_C'");
	case 4:
		return TEXT("Blueprint'/Game/Assets/Fruits/Nut/Fruit_Nut.Fruit_Nut_C'");
	case 5:
		return TEXT("Blueprint'/Game/Assets/Fruits/Durian/Fruit_Durian.Fruit_Durian_C'");
	default:
		return TEXT("None");
	}
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
	case 4:
		res = LoadObject<UTexture2D>(NULL, TEXT("/Game/Assets/Fruits/Nut/Icon_nut.Icon_nut"), NULL, LOAD_None, NULL);
		break;
	case 5:
		res = LoadObject<UTexture2D>(NULL, TEXT("/Game/Assets/Fruits/Durian/Icon_durian.Icon_durian"), NULL, LOAD_None, NULL);
	default:
		break;
	}
	return res;
}
