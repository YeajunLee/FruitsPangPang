// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AI_Sword_Controller_Custom.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API AAI_Sword_Controller_Custom : public AAIController
{
	GENERATED_BODY()

public:
	AAI_Sword_Controller_Custom();
	virtual void OnPossess(APawn* InPawn) override;

	static const FName SwordTargetKey;


private:
	UPROPERTY();
	class UBehaviorTree* BTAsset;

	UPROPERTY();
	class UBlackboardData* BBAsset;
	
};
