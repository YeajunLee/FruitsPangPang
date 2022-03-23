// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_FindTreePos.generated.h"

/**
 * 
 */
UCLASS()
class AICLIENT_API UBTTaskNode_FindTreePos : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskNode_FindTreePos();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
};
