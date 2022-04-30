// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_Farming.h"
#include "AICharacter.h"
#include "AIController_Custom.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Inventory.h"
#include "Tree.h"

UBTTaskNode_Farming::UBTTaskNode_Farming()
{
	NodeName = TEXT("CPP_BTTFarming");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskNode_Farming::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	auto ai = Cast<AAICharacter>(ControllingPawn);
	ai->GetFruits();

	//int FruitAmount = ai->mInventory->mSlots[ai->SelectedHotKeySlotNum].Amount;
	//OwnerComp.GetBlackboardComponent()->SetValueAsInt(AAIController_Custom::AmountKey, FruitAmount);

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAIController_Custom::TargetKey, nullptr);

	//ATree* targetTree = static_cast<ATree*>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAIController_Custom::TreePosKey));

	//if (targetTree->CanHarvest == false) {
	//	//FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	//	//UE_LOG(LogTemp, Warning, TEXT("CAN NOT HARVEST!"));
	//	return EBTNodeResult::Failed;

	//}
	return EBTNodeResult::InProgress;
}

void UBTTaskNode_Farming::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	auto AICharacter = Cast<AAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == AICharacter)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	if (OwnerComp.GetBlackboardComponent()->GetValueAsInt(AAIController_Custom::AmountKey) > 0)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	ATree* targetTree = static_cast<ATree*>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAIController_Custom::TreePosKey));
	
	if (targetTree->CanHarvest == false) {
		//UE_LOG(LogTemp, Log, TEXT("called %d"), AICharacter->c_id);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		//UE_LOG(LogTemp, Warning, TEXT("CAN NOT HARVEST!"));
	}
	else
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

}