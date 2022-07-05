// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_ItemCodeCheck.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AICharacter.h"
#include "AI_Sword_Controller_Custom.h"
#include "AI_Smart_Controller_Custom.h"


UBTDecorator_ItemCodeCheck::UBTDecorator_ItemCodeCheck()
{
	NodeName = TEXT("ItemCodeChecker");

}

bool UBTDecorator_ItemCodeCheck::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	auto ai = Cast<AAICharacter>(ControllingPawn);

	auto swordAIController = Cast<AAI_Sword_Controller_Custom>(ai->Controller);
	auto smartAIController = Cast<AAI_Smart_Controller_Custom>(ai->Controller);

	int itemCode = -1;
	if (swordAIController)
	{
		itemCode = OwnerComp.GetBlackboardComponent()->GetValueAsInt(AAI_Sword_Controller_Custom::PunnetItemKey);

	}
	else if (smartAIController)
	{
		itemCode = OwnerComp.GetBlackboardComponent()->GetValueAsInt(AAI_Smart_Controller_Custom::PunnetItemKey);

	}
	bool bResult = (itemCode == 7 || itemCode == 8);
	return bResult;

}
