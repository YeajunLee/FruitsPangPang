// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SwordAttack.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UBTTask_SwordAttack : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_SwordAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;
	
private:
	bool IsAttacking = false;
};
