// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_GoToTheTree.h"
#include "AIController_Custom.h"
#include "BehaviorTree/BlackboardComponent.h"
//#include "NavigationSystem.h"
#include "AICharacter.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

UBTTaskNode_GoToTheTree::UBTTaskNode_GoToTheTree()
{
	NodeName = TEXT("CPP_BTTGoToTheTree");
}

EBTNodeResult::Type UBTTaskNode_GoToTheTree::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	UObject* TreePos = OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAIController_Custom::TreePosKey);
	UAIBlueprintHelperLibrary::SimpleMoveToActor(ControllingPawn->GetController(), static_cast<AActor*>(TreePos));

	AAICharacter* ai = Cast<AAICharacter>(ControllingPawn);

	if (ai->bIsDie) EBTNodeResult::Failed;
	if (ai->bIsUndertheTree)
		return EBTNodeResult::Succeeded;
	return EBTNodeResult::Failed;

}