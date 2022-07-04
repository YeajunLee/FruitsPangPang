// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include <vector>
#include "BTTaskNode_HealFinding.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UBTTaskNode_HealFinding : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskNode_HealFinding();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
