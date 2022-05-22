// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include <vector>
#include "BTTask_PunnetFinding.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UBTTask_PunnetFinding : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_PunnetFinding();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
