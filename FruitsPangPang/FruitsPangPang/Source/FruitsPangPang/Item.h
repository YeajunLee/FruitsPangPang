// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Item.generated.h"


USTRUCT(BlueprintType)
struct FStoreItemInfo : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Store")
		int ItemCode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Store")
		FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Store")
		int Price;
};

//����ü �տ� F ���̴°� ��۸� ������ ������ ----------�̰� InGame���� ���Ǵ� �������̴�.  Atomic�� �� ����ü�� �׻� �ϳ��� ������ ����ȭ(Serialize)���� �ǹ�
USTRUCT(Atomic, BlueprintType)
struct FItemInfo
{
	GENERATED_BODY()
public:
	FItemInfo()
		: Name()
		, Icon()
		, Mesh()
		, InteractVolumeRadious()
		, IndexOfHotKeySlot()
		, ItemCode()
		, MaxStackAmount()
	{

	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
		FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
		UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
		UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
		float InteractVolumeRadious;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
		int IndexOfHotKeySlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
		int ItemCode;	//0:None, 1:tomato, 2:kiwi, 3:apple, 4:subak, 5:pineapple, 6:pumpkin, 7:greenonion, 8:carrot, 9:durian, 10:nut, 11:banana, 12:heal

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
		int MaxStackAmount;
};


UCLASS()
class FRUITSPANGPANG_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	class AInventory* mInventory;

	//�����ͷ� ���ϱ� �ߴµ� ��� ���� �𸣰���. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		FItemInfo mItemInfo;

};

