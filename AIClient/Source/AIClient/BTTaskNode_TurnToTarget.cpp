// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_TurnToTarget.h"
#include "AICharacter.h"
#include "AIController_Custom.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MyCharacter.h"
#include "Math/UnrealMathUtility.h"

UBTTaskNode_TurnToTarget::UBTTaskNode_TurnToTarget()
{
	NodeName = TEXT("Turn");
}

EBTNodeResult::Type UBTTaskNode_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	auto AICharacter = Cast <AAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == AICharacter)
	{
		return EBTNodeResult::Failed;
	}
	auto Target = Cast<AMyCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAIController_Custom::TargetKey));
	if (nullptr == Target)
		return EBTNodeResult::Failed;

	FVector LookVector = Target->GetActorLocation() - AICharacter->GetActorLocation();
	LookVector.Z = 0.f;

	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	AICharacter->SetActorRotation(FMath::RInterpTo(
		AICharacter->GetActorRotation(),
		TargetRot, GetWorld()->GetDeltaSeconds(),
		2.f));
	
	return EBTNodeResult::Succeeded;
}

