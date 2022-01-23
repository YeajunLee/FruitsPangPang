// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"


//����ü �տ� F ���̴°� ��۸� ������ ������
USTRUCT(Atomic, BlueprintType)
struct FItemInfo
{
	GENERATED_BODY()
public:
	FItemInfo()
		: Name()
		,Icon()
		,Mesh()
		,InteractVolumeRadious()
		,IndexOfHotKeySlot()
		,ItemCode()
		,MaxStackAmount()
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
	int ItemCode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	int MaxStackAmount;
};


UCLASS()
class FPS_CPP_API AItem : public AActor
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

