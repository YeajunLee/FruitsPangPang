// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIControllerCustom.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API AAIControllerCustom : public AAIController
{
	GENERATED_BODY()
	
public:
	AAIControllerCustom();
	virtual void OnPossess(APawn* InPawn) override;

	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetKey;

private:
	UPROPERTY();
	class UBehaviorTree* BTAsset;

	UPROPERTY();
	class UBlackboardData* BBAsset;

	
};
