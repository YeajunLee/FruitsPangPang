// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SwordAttack.h"
//#include "AI_SwordCharacter.h"
#include "AICharacter.h"
#include "AI_Sword_Controller_Custom.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MyCharacter.h"

UBTTask_SwordAttack::UBTTask_SwordAttack()
{
	NodeName = TEXT("CPP_BTTSwordAttack");

	bNotifyTick = true; // 틱 기능 활성화?
	IsAttacking = false;
}

EBTNodeResult::Type UBTTask_SwordAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAICharacter* swordAI = Cast<AAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == swordAI)
		return EBTNodeResult::Failed;

	auto Target = Cast<AMyCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAI_Sword_Controller_Custom::SwordTargetKey));
	//UE_LOG(LogTemp, Warning, TEXT("%s"), Target);
	if (nullptr == Target)
		return EBTNodeResult::Failed;
	if (Target->bIsDie)
		return EBTNodeResult::Failed;
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
