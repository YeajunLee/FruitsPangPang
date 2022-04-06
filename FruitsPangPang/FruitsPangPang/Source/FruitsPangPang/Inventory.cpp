// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"
#include "MainWidget.h"
#include "BaseCharacter.h"
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
			mMainWidget->mOwnerCharacter = mOwnerCharacter;
			for (int i = 0; i < 5; ++i)
			{
				auto slot = CreateWidget<UInventorySlotWidget>(GetWorld(), mMakerInventorySlotWidget);
				slot->inventoryRef = this;
				slot->mIndex = i;
				slot->Update();
				mMainWidget->InventoryBar->AddChildToHorizontalBox(slot);
				mMainWidget->minventorySlot.Add(slot);
			}
			mMainWidget->AddToViewport();//Nativecontruct 호출 시점임.
			mMainWidget->SetVisibility(ESlateVisibility::Visible);
			mMainWidget->minventorySlot[0]->Select();
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

	mMainWidget->minventorySlot[item.IndexOfHotKeySlot]->Update(); //<- 이런식으로 바뀔 예정
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

	mMainWidget->minventorySlot[item.IndexOfHotKeySlot]->Update();// <- 이런식으로 바뀔 예정
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

int AInventory::GetItemCodeAtSlotIndex(const int& index)
{
	bool isempty = !IsSlotValid(index);
	if (!isempty)
	{
		return mSlots[index].ItemClass.ItemCode;
	}

	return 0;	//invalid code 

}

void AInventory::RemoveItemAtSlotIndex(const int& index, const int& amount)
{
	if (amount <= 0 || !IsSlotValid(index)) return;

	if (mSlots[index].Amount > amount)
	{
		mSlots[index].Amount -= amount;
		mMainWidget->minventorySlot[index]->Update(); //<- 이런식으로 바뀔 예정
		//mInventoryMainWidget->minventorySlot->Update();
		return;
	}

	mSlots[index].Amount = 0;
	mSlots[index].ItemClass = FItemInfo();
	mMainWidget->minventorySlot[index]->Update(); //<- 이런식으로 바뀔 예정
	//mInventoryMainWidget->minventorySlot->Update();

}

void AInventory::ClearInventory()
{
	for (auto& slot : mSlots)
	{
		slot.Amount = 0;
		slot.ItemClass = FItemInfo();
	}
	for (auto& slot : mMainWidget->minventorySlot)
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
		res = FText::FromString(FString("Apple"));
		break;
	case 4:
		res = FText::FromString(FString("Watermelon"));
		break;
	case 5:
		res = FText::FromString(FString("PineApple"));
		break;
	case 6:
		res = FText::FromString(FString("Pumpkin"));
		break;
	case 7:
		res = FText::FromString(FString("GreenOnion"));
		break;
	case 8:
		res = FText::FromString(FString("Carrot"));
		break;
	case 9:
		res = FText::FromString(FString("Durian"));
		break;
	case 10:
		res = FText::FromString(FString("Nut"));
		break;
	case 11:
		res = FText::FromString(FString("Banana"));
		break;
	case 12:
		res = FText::FromString(FString("FirstAidKit"));
		break;
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
		return TEXT("Blueprint'/Game/Assets/Fruits/tomato/Bomb_Test.Bomb_Test_C'");
	case 2:
		return TEXT("Blueprint'/Game/Assets/Fruits/Kiwi/KiwiBomb.KiwiBomb_C'");
	case 3:
		return TEXT("Blueprint'/Game/Assets/Fruits/Apple/DM_Apple_BP.DM_Apple_BP_C'");
	case 4:
		return TEXT("Blueprint'/Game/Assets/Fruits/WaterMelon/DM_Watermelon_BP.DM_Watermelon_BP_C'");
	case 5:
		return TEXT("Blueprint'/Game/Assets/Fruits/Pineapple/DM_Pineapple_BP.DM_Pineapple_BP_C'");
	case 6:
		return TEXT("Blueprint'/Game/Assets/Fruits/Pumkin/DM_Pumpkin_BP.DM_Pumpkin_BP_C'");
	case 7:
		return TEXT("None");
	case 8:
		return TEXT("None");
	case 9:
		return TEXT("Blueprint'/Game/Assets/Fruits/Durian/DurianBomb.DurianBomb_C'");
	case 10:
		return TEXT("Blueprint'/Game/Assets/Fruits/Nut/NutBomb.NutBomb_C'");
	case 11:
		return TEXT("Blueprint'/Game/Assets/Fruits/Banana/BananaBomb.BananaBomb_C'");
	case 12:
		return TEXT("None");
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
		return TEXT("Blueprint'/Game/Assets/Fruits/Apple/Fruit_Apple.Fruit_Apple_C'");
	case 4:
		return TEXT("Blueprint'/Game/Assets/Fruits/Watermelon/Fruit_Watermelon.Fruit_Watermelon_C'");
	case 5:
		return TEXT("Blueprint'/Game/Assets/Fruits/Pineapple/Fruit_Pineapple.Fruit_Pineapple_C'");
	case 6:
		return TEXT("Blueprint'/Game/Assets/Fruits/Pumkin/Fruit_Pumpkin.Fruit_Pumpkin_C'");
	case 7:
		return TEXT("Blueprint'/Game/Assets/Fruits/BigGreenOnion/Fruit_GreenOnion.Fruit_GreenOnion_C'");
	case 8:
		return TEXT("Blueprint'/Game/Assets/Fruits/Carrot/Fruit_Carrot.Fruit_Carrot_C'");
	case 9:
		return TEXT("Blueprint'/Game/Assets/Fruits/Durian/Fruit_Durian.Fruit_Durian_C'");
	case 10:
		return TEXT("Blueprint'/Game/Assets/Fruits/Nut/Fruit_Nut.Fruit_Nut_C'");
	case 11:
		return TEXT("Blueprint'/Game/Assets/Fruits/Banana/Fruit_Banana.Fruit_Banana_C'");
	case 12:
		return TEXT("Blueprint'/Game/Assets/MapObjects/FirstAidKit/FirstAidKit.FirstAidKit_C'");
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
		res = LoadObject<UTexture2D>(NULL, TEXT("/Game/Assets/Fruits/tomato/Textures/Tomato_slot.Tomato_slot"), NULL, LOAD_None, NULL);
		break;
	case 2:
		res = LoadObject<UTexture2D>(NULL, TEXT("/Game/Assets/Fruits/Kiwi/Kiwi_slot.Kiwi_slot"), NULL, LOAD_None, NULL);
		break;
	case 3:
		res = LoadObject<UTexture2D>(NULL, TEXT("/Game/Assets/Fruits/Apple/Apple_slot.Apple_slot"), NULL, LOAD_None, NULL);
		break;
	case 4:
		res = LoadObject<UTexture2D>(NULL, TEXT("/Game/Assets/Fruits/Watermelon/Watermelon_slot.Watermelon_slot"), NULL, LOAD_None, NULL);
		break;
	case 5:
		res = LoadObject<UTexture2D>(NULL, TEXT("/Game/Assets/Fruits/Pineapple/Pineapple_slot.Pineapple_slot"), NULL, LOAD_None, NULL);
		break;
	case 6:
		res = LoadObject<UTexture2D>(NULL, TEXT("/Game/Assets/Fruits/Pumkin/Pumpkin_slot.Pumpkin_slot"), NULL, LOAD_None, NULL);
		break;
	case 7:
		res = LoadObject<UTexture2D>(NULL, TEXT("/Game/Assets/Fruits/BigGreenOnion/Leek_slot.Leek_slot"), NULL, LOAD_None, NULL);
		break;
	case 8:
		res = LoadObject<UTexture2D>(NULL, TEXT("/Game/Assets/Fruits/Carrot/Carrot_slot.Carrot_slot"), NULL, LOAD_None, NULL);
		break;
	case 9:
		res = LoadObject<UTexture2D>(NULL, TEXT("/Game/Assets/Fruits/Durian/Doorian_slot.Doorian_slot"), NULL, LOAD_None, NULL);
		break;
	case 10:
		res = LoadObject<UTexture2D>(NULL, TEXT("/Game/Assets/Fruits/Nut/Bam_slot.Bam_slot"), NULL, LOAD_None, NULL);
		break;
	case 11:
		res = LoadObject<UTexture2D>(NULL, TEXT("/Game/Assets/Fruits/Banana/Banana_slot.Banana_slot"), NULL, LOAD_None, NULL);
		break;
	case 12:
		break;
	default:
		break;
	}
	return res;
}