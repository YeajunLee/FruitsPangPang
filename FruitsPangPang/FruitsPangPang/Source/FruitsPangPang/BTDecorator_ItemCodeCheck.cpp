// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_ItemCodeCheck.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI_Sword_Controller_Custom.h"


UBTDecorator_ItemCodeCheck::UBTDecorator_ItemCodeCheck()
{
	NodeName = TEXT("ItemCodeChecker");

}

bool UBTDecorator_ItemCodeCheck::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	int itemCode = OwnerComp.GetBlackboardComponent()->GetValueAsInt(AAI_Sword_Controller_Custom::PunnetItemKey);

	bool bResult = (itemCode == 7 || itemCode == 8);
	return bResult;
}
