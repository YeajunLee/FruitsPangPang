// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckHP.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UBTDecorator_CheckHP : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTDecorator_CheckHP();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
