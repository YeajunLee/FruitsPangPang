// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_Farming.generated.h"

/**
 * 
 */
UCLASS()
class AICLIENT_API UBTTaskNode_Farming : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskNode_Farming();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;

};
