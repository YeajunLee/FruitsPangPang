// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SwordAttack.h"
//#include "AI_SwordCharacter.h"
#include "AICharacter.h"
#include "AI_Sword_Controller_Custom.h"
#include "AI_Smart_Controller_Custom.h"
#include "BehaviorTree/BlackboardComponent.h"
//#include "MyCharacter.h"

UBTTask_SwordAttack::UBTTask_SwordAttack()
{
	NodeName = TEXT("CPP_BTTSwordAttack");

	bNotifyTick = true; // ƽ ???? Ȱ??ȭ?
	IsAttacking = false;
}

EBTNodeResult::Type UBTTask_SwordAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAICharacter* swordAI = Cast<AAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == swordAI)
		return EBTNodeResult::Failed;

	auto swordAIController = Cast<AAI_Sword_Controller_Custom>(swordAI->Controller);
	auto smartAIController = Cast<AAI_Smart_Controller_Custom>(swordAI->Controller);

	if (smartAIController)
	{
		if (swordAI->SelectedHotKeySlotNum != 2)
			swordAI->PickSwordAnimation();
		swordAI->SelectedHotKeySlotNum = 2;
	}

	ABaseCharacter* Target = nullptr;

	if(swordAIController)
		Target = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAI_Sword_Controller_Custom::SwordTargetKey));
	else if(smartAIController)
		Target = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAI_Smart_Controller_Custom::TargetKey));

	if (nullptr == Target)
		return EBTNodeResult::Failed;
	if (Target->bIsDie) {
		if (swordAIController)
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAI_Sword_Controller_Custom::SwordTargetKey, nullptr);
		else if (smartAIController)
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAI_Smart_Controller_Custom::TargetKey, nullptr);
		return EBTNodeResult::Failed;
	}
	if (swordAI->bIsDie)
		return EBTNodeResult::Failed;

	if (swordAI->bStepBanana == false)
		swordAI->Attack();
	IsAttacking = true;
	swordAI->OnAttackEnd.AddLambda([this]()->void {IsAttacking = false; });

	return EBTNodeResult::InProgress;

}

void UBTTask_SwordAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAICharacter* swordAI = Cast<AAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == swordAI)
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	if (!IsAttacking)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
