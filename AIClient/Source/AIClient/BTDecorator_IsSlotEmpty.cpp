// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsSlotEmpty.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Projectile.h"
#include "Tree.h"
#include "AIController_Custom.h"
#include "AICharacter.h"
#include "Inventory.h"

UBTDecorator_IsSlotEmpty::UBTDecorator_IsSlotEmpty()
{
	NodeName = TEXT("CanNotThrow");
}


bool UBTDecorator_IsSlotEmpty::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	int ai_amount = OwnerComp.GetBlackboardComponent()->GetValueAsInt(AAIController_Custom::AmountKey);

	//남은 토마토가 0이되면 true
	bool bResult = ai_amount <= 0;

	if (bResult)
		UE_LOG(LogTemp, Warning, TEXT("tomato empty"));


	return bResult;
}
