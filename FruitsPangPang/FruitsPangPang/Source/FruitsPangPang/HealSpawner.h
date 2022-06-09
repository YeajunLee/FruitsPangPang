// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction.h"
#include "HealSpawner.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API AHealSpawner : public AInteraction
{
	GENERATED_BODY()
public:
	AHealSpawner();

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interact | HealSpawner")
		int HealSpanwerId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interact | HealSpawner")
		int FruitType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "interact | HealSpawner")
		bool CanHarvest;	//���, �������� ��Ȯ������ �����ϱ⶧���� �ʿ������ ���ʿ��� ��Ŷ�� �����°� ���� ���ؼ� �������.

	void GenerateFruit(int _FruitType);
	void HarvestFruit();

	class AFruit* mFruitMesh;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//
	virtual void EndPlay(EEndPlayReason::Type Reason) override;
};
