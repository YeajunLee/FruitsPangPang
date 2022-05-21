// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SwordFarming.h"
//#include "AI_SwordCharacter.h"
#include "AICharacter.h"
#include "AI_Sword_Controller_Custom.h"
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

	OwnerComp.GetBlackboardComponent()->SetValueAsInt(AAI_Sword_Controller_Custom::PunnetItemKey, swordAIController->SavedItemCode);
	//auto swordAIController = Cast<AAI_Sword_Controller_Custom>(PacketOwner->Controller);
	//if ((packet->itemCode == 7 || packet->itemCode == 8))
	//	swordAIController->BrainComponent->GetBlackboardComponent()->SetValueAsInt(AAI_Sword_Controller_Custom::PunnetItemKey, packet->itemCode);

	//OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAI_Sword_Controller_Custom::SwordTargetKey, nullptr);

	// fail하고 다시 blackboard에서 key값으로 어디로갈지 결정하기 위해 key값 세팅
	//OwnerComp.GetBlackboardComponent()->SetValueAsInt(AAI_Sword_Controller_Custom::PunnetItemKey, getWhat);
	

	return EBTNodeResult::InProgress;
}

void UBTTask_SwordFarming::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	auto AICharacter = Cast<AAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == AICharacter)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

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
