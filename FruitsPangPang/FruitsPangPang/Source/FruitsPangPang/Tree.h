// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction.h"
#include "Tree.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API ATree : public AInteraction
{
	GENERATED_BODY()
public:
	ATree();

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interact | Tree")
		int TreeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interact | Tree")
		int FruitType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "interact | Tree")
		bool CanHarvest;	//���, �������� ��Ȯ������ �����ϱ⶧���� �ʿ������ ���ʿ��� ��Ŷ�� �����°� ���� ���ؼ� �������.

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class  UPointOfInterestComponent* POIcomp;

	void GenerateFruit(int _FruitType);
	void HarvestFruit();

	class AFruit* mFruitMesh[3];

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
		class USoundBase* HarvestTreeSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
		class USoundBase* HarvestTreeSound2;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//
	virtual void EndPlay(EEndPlayReason::Type Reason) override;
	
};
