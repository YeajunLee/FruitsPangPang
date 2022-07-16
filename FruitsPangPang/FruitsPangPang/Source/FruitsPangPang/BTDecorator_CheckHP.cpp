// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_CheckHP.h"
#include "AIController_Custom.h"
#include "AICharacter.h"

UBTDecorator_CheckHP::UBTDecorator_CheckHP()
{
	NodeName = TEXT("CPP_HPChecker");

}

bool UBTDecorator_CheckHP::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return false;

	auto ai = Cast<AAICharacter>(ControllingPawn);

	bool bResult = (ai->hp <= 6);

	return bResult;
}
