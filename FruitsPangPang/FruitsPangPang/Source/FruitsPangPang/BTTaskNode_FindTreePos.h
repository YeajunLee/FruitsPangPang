// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_FindTreePos.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UBTTaskNode_FindTreePos : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	float currentClosestDistance;
	UBTTaskNode_FindTreePos();

	AActor* GetClosestActor(FVector sourceLocation, TArray<AActor*> actors);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
};
