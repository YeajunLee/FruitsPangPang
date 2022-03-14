// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsSlotEmpty.generated.h"

/**
 * 
 */
UCLASS()
class AICLIENT_API UBTDecorator_IsSlotEmpty : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_IsSlotEmpty();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Select")
	int SelectedHotKeySlotNum;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	class AInventory* mInventory;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
