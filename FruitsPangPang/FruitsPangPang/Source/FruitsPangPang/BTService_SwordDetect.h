// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BTService_Detect.h"
#include "BTService_SwordDetect.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UBTService_SwordDetect : public UBTService_Detect
{
	GENERATED_BODY()
public:
	UBTService_SwordDetect();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	
};
