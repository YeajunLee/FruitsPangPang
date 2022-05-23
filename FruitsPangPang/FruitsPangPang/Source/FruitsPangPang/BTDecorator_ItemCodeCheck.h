// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_ItemCodeCheck.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UBTDecorator_ItemCodeCheck : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_ItemCodeCheck();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
