// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SwordFarming.h"
//#include "AI_SwordCharacter.h"
#include "AICharacter.h"
#include "AI_Sword_Controller_Custom.h"
#include "AI_Smart_Controller_Custom.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Inventory.h"
#include "Punnet.h"

UBTTask_SwordFarming::UBTTask_SwordFarming()
{
	NodeName = TEXT("SwordFarming");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_SwordFarming::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	auto swordAI = Cast<AAICharacter>(ControllingPawn);

	if(swordAI->bIsDie)
		return EBTNodeResult::Failed;
	swordAI->GetFruits();

	//sword_ai
	auto swordAIController = Cast<AAI_Sword_Controller_Custom>(swordAI->Controller);
	auto smartAIController = Cast<AAI_Smart_Controller_Custom>(swordAI->Controller);

	if(swordAIController)
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(AAI_Sword_Controller_Custom::PunnetItemKey, swordAIController->SavedItemCode);
	else if(smartAIController)
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(AAI_Smart_Controller_Custom::PunnetItemKey, smartAIController->SavedItemCode);


	return EBTNodeResult::InProgress;
}

void UBTTask_SwordFarming::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	auto AICharacter = Cast<AAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == AICharacter)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	//2022-07-05
	auto swordAIController = Cast<AAI_Sword_Controller_Custom>(AICharacter->Controller);
	auto smartAIController = Cast<AAI_Smart_Controller_Custom>(AICharacter->Controller);

	if (swordAIController)
	{
		int itemCode = OwnerComp.GetBlackboardComponent()->GetValueAsInt(AAI_Sword_Controller_Custom::PunnetItemKey);
		//UE_LOG(LogTemp, Warning, TEXT("%d"), itemCode);
		// 대파나 당근을 get했으면
		if (7 == itemCode || 8 == itemCode) {
			//Succeeded
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
		else { //아니라면
			// 다시 파밍
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
	}
	else if (smartAIController)
	{
		int itemCode = OwnerComp.GetBlackboardComponent()->GetValueAsInt(AAI_Smart_Controller_Custom::PunnetItemKey);
		if (7 == itemCode || 8 == itemCode) {
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
		else { 
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
	}
	
}
