// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction.h"
#include "Tree.generated.h"

/**
 * 
 */
UCLASS()
class FPS_CPP_API ATree : public AInteraction
{
	GENERATED_BODY()
public:
	ATree();

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interact")
	int TreeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "interact")
	int FruitType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "interact")
	bool CanHarvest;

	void GenerateFruit(int _FruitType);
	void HarvestFruit();

	class ATestTree* mFruitMesh[3];
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//
	virtual void EndPlay(EEndPlayReason::Type Reason) override;
};
