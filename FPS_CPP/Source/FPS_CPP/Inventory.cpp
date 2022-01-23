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
