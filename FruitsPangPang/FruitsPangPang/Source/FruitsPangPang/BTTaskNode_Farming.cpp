// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_Farming.h"
#include "AICharacter.h"
#include "AIController_Custom.h"
#include "AI_Smart_Controller_Custom.h"
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

	if (ai->bIsDie)
		return EBTNodeResult::Failed;
	ai->GetFruits();

	//2022-07-05
	auto AIController = Cast<AAIController_Custom>(ai->Controller);
	auto smartAIController = Cast<AAI_Smart_Controller_Custom>(ai->Controller);

	if(AIController)
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAIController_Custom::TargetKey, nullptr);
	else if(smartAIController)
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAI_Smart_Controller_Custom::TargetKey, nullptr);


	return EBTNodeResult::InProgress;
}

void UBTTaskNode_Farming::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	auto AICharacter = Cast<AAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == AICharacter)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	//2022-07-05
	auto AIController = Cast<AAIController_Custom>(AICharacter->Controller);
	auto smartAIController = Cast<AAI_Smart_Controller_Custom>(AICharacter->Controller);

	if (AIController)
	{
		if (OwnerComp.GetBlackboardComponent()->GetValueAsInt(AAIController_Custom::AmountKey) > 0)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}

		ATree* targetTree = static_cast<ATree*>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAIController_Custom::TreePosKey));
		if (targetTree->CanHarvest == false)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
		else
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	else if (smartAIController)
	{
		if (OwnerComp.GetBlackboardComponent()->GetValueAsInt(AAI_Smart_Controller_Custom::AmountKey) > 0)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}

		ATree* targetTree = static_cast<ATree*>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAI_Smart_Controller_Custom::TreePosKey));
		if (targetTree->CanHarvest == false)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
		else
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	

	
}