// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_TargetTracking.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UBTService_TargetTracking : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_TargetTracking();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	
};
