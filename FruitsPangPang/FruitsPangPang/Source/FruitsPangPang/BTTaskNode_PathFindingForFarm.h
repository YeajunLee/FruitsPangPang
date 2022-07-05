// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include <vector>
#include "BTTaskNode_PathFindingForFarm.generated.h"


/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UBTTaskNode_PathFindingForFarm : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskNode_PathFindingForFarm();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;

	//void FindTree();
};
