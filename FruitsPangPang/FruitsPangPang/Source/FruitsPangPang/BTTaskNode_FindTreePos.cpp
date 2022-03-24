// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_FindTreePos.h"
#include "AIController_Custom.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tree.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"


UBTTaskNode_FindTreePos::UBTTaskNode_FindTreePos()
{
	NodeName = TEXT("CPP_BTTFindTreePos");
}

EBTNodeResult::Type UBTTaskNode_FindTreePos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AActor* trees = UGameplayStatics::GetActorOfClass(GetWorld(), ATree::StaticClass());
	FVector TreePos;
	TreePos = trees->GetActorLocation();

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(AAIController_Custom::TreePosKey, TreePos);

	//UAIBlueprintHelperLibrary::SimpleMoveToLocation(OwnerComp.GetAIOwner()->GetPawn()->GetController(), TreePos);

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *OwnerComp.GetBlackboardComponent()->GetValueAsVector(AAIController_Custom::TreePosKey).ToString());

	return EBTNodeResult::Succeeded;
}
