// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_FindTreePos.generated.h"
/**
 * 
 */
class ATree;
UCLASS()
class FRUITSPANGPANG_API UBTTaskNode_FindTreePos : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	float currentClosestDistance;
	UBTTaskNode_FindTreePos();

	ATree* GetClosestTree(FVector sourceLocation, std::vector<ATree*> actors);

	//void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);


	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
};
