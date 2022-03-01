// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction.h"
#include "Punnet.generated.h"

/**
 * 
 */
UCLASS()
class AICLIENT_API APunnet : public AInteraction
{
	GENERATED_BODY()
public:
	APunnet();


	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interact | Punnet")
		int PunnetId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interact | Punnet")
		int FruitType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "interact | Punnet")
		bool CanHarvest;	//���, �������� ��Ȯ������ �����ϱ⶧���� �ʿ������ ���ʿ��� ��Ŷ�� �����°� ���� ���ؼ� �������.

	void GenerateFruit(int _FruitType);
	void HarvestFruit();

	class AFruit* mFruitMesh[3];
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//
	virtual void EndPlay(EEndPlayReason::Type Reason) override;
};
