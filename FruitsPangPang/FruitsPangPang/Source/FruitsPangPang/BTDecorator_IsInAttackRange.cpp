// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsInAttackRange.h"
#include "AIController_Custom.h"
#include "AI_Smart_Controller_Custom.h"
#include "AI_Sword_Controller_Custom.h"
#include "AICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
//#include "MyCharacter.h"

UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
	NodeName = TEXT("CPP_BTTCanAttack");
}


bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return false;

	//2022-07-05
	AAICharacter* ai = Cast<AAICharacter>(ControllingPawn);

	auto AIController = Cast<AAIController_Custom>(ai->Controller);
	auto smartAIController = Cast<AAI_Smart_Controller_Custom>(ai->Controller);

	//2022-07-19 swordAIµµ isAttackRange »ç¿ë.
	auto swordAIController = Cast<AAI_Sword_Controller_Custom>(ai->Controller);


	ABaseCharacter* Target = nullptr;
	if(AIController)
		Target = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAIController_Custom::TargetKey));
	else if(smartAIController)
		Target = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAI_Smart_Controller_Custom::TargetKey));
	else if(swordAIController)
		Target = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAI_Sword_Controller_Custom::SwordTargetKey));

	if (nullptr == Target) return false;

	bool bResult = (Target->GetDistanceTo(ControllingPawn) <= 600.0f);
	return bResult;
}