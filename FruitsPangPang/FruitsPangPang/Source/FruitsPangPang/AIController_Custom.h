// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include <vector>
#include "AIController_Custom.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API AAIController_Custom : public AAIController
{
	GENERATED_BODY()

public:
	AAIController_Custom();
	virtual void OnPossess(APawn* InPawn) override;

	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetKey;

	static FName TreePosKey;
	static FName AmountKey;
	static FName TargetTreeKey;
	static FName TrackingTargetKey;

private:
	UPROPERTY();
	class UBehaviorTree* BTAsset;

	UPROPERTY();
	class UBlackboardData* BBAsset;
	
};
