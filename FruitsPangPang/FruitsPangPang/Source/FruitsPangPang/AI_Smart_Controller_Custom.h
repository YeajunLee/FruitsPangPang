// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AI_Smart_Controller_Custom.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API AAI_Smart_Controller_Custom : public AAIController
{
	GENERATED_BODY()

public:
	AAI_Smart_Controller_Custom();
	virtual void OnPossess(APawn* InPawn) override;

	static const FName TargetKey;

	static FName PunnetKey;
	static FName PunnetItemKey;

	static FName TreePosKey;
	static FName AmountKey;
	static FName TrackingTargetKey;


private:
	UPROPERTY();
	class UBehaviorTree* BTAsset;

	UPROPERTY();
	class UBlackboardData* BBAsset;
	
};
