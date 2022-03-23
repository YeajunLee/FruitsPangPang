// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_Farming.h"
#include "AICharacter.h"
#include "AIController_Custom.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Inventory.h"

UBTTaskNode_Farming::UBTTaskNode_Farming()
{
	NodeName = TEXT("CPP_BTTFarming");
}

EBTNodeResult::Type UBTTaskNode_Farming::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	auto ai = Cast<AAICharacter>(ControllingPawn);
	ai->GetFruits();

	int FruitAmount = ai->mInventory->mSlots[ai->SelectedHotKeySlotNum].Amount;
	OwnerComp.GetBlackboardComponent()->SetValueAsInt(AAIController_Custom::AmountKey, FruitAmount);

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAIController_Custom::TargetKey, nullptr);

	return EBTNodeResult::Succeeded;
}