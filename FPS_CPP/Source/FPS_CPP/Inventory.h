// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.h"
#include "Inventory.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()
public:
	FInventorySlot()
		: ItemClass()
		, Amount()
	{

	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	FItemInfo ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	int Amount;
};


UCLASS()
class FPS_CPP_API AInventory : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInventory();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Default", meta = (ExposeOnSpawn = "true"))
	class AMyCharacter* mCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TArray<FInventorySlot> mSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (ExposeOnSpawn = "true"))
	int mAmountOfSlots;

	UPROPERTY(EditAnywhere, blueprintReadOnly, Category = "MainWidget", Meta = (AllowPrivateAccess = true))
		TSubclassOf<class UMainWidget> mMakerMainWidget;
	class UMainWidget* mMainWidget;

	void AddItem(const FItemInfo& item, const int& amount);
	void UpdateInventorySlot(const FItemInfo& item, const int& amount);
	void GetItemInfoAtSlotIndex(__in const int& index, __out bool& isempty, __out FItemInfo& iteminfo, __out int& amount);
	void RemoveItemAtSlotIndex(const int& index, const int& amount);
	bool IsSlotValid(const int& index);
	const FText ItemCodeToItemName(const int& itemCode);
};

