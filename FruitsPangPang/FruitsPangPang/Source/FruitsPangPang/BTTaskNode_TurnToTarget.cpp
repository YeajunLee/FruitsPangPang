// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_TurnToTarget.h"
#include "AICharacter.h"
#include "AIController_Custom.h"
#include "AI_Smart_Controller_Custom.h"
#include "BehaviorTree/BlackboardComponent.h"
//#include "MyCharacter.h"
#include "Math/UnrealMathUtility.h"

UBTTaskNode_TurnToTarget::UBTTaskNode_TurnToTarget()
{
	NodeName = TEXT("CPP_BTTTurn");
}

EBTNodeResult::Type UBTTaskNode_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	auto AICharacter = Cast <AAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == AICharacter)
	{
		return EBTNodeResult::Failed;
	}

	if (AICharacter->bIsDie)
		EBTNodeResult::Failed;

	//2022-07-05
	auto AIController = Cast<AAIController_Custom>(AICharacter->Controller);
	auto smartAIController = Cast<AAI_Smart_Controller_Custom>(AICharacter->Controller);

	ABaseCharacter* Target = nullptr;
	if(AIController)
		Target = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAIController_Custom::TargetKey));
	else if(smartAIController)
		Target = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAI_Smart_Controller_Custom::TargetKey));

	if (nullptr == Target)
		return EBTNodeResult::Failed;

	FVector LookVector = Target->GetActorLocation() - AICharacter->GetActorLocation();
	LookVector.Z = 0.f;

	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	AICharacter->SetActorRotation(TargetRot);
	//AICharacter->SetActorRotation(FMath::RInterpTo(
	//	AICharacter->GetActorRotation(),
	//	TargetRot, GetWorld()->GetDeltaSeconds(),
	//	2.f));

	
	return EBTNodeResult::Succeeded;
}

