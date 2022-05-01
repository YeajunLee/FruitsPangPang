// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_FindTreePos.h"
#include "AIController_Custom.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tree.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "DrawDebugHelpers.h"
#include "AICharacter.h"
#include <algorithm>


UBTTaskNode_FindTreePos::UBTTaskNode_FindTreePos()
{
	NodeName = TEXT("CPP_BTTFindTreePos");

	currentClosestDistance = TNumericLimits<float>::Max();
	//bNotifyTick = true;
}

ATree* UBTTaskNode_FindTreePos::GetClosestTree(FVector sourceLocation, std::vector<ATree*> actors)
{
	if (actors.size() <= 0)
		return nullptr;


	ATree* closestActor = actors[0];
	for (int i = 0; i < actors.size(); ++i)
	{
		float distance = FVector::DistSquared(sourceLocation, actors[i]->GetActorLocation());
		if (actors[i]->CanHarvest)
		{
			if (distance < currentClosestDistance)
			{
				currentClosestDistance = distance;				
				closestActor = actors[i];
			}
		}
	}
	currentClosestDistance = TNumericLimits<float>::Max();

	return closestActor;
}

EBTNodeResult::Type UBTTaskNode_FindTreePos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	UWorld* World = ControllingPawn->GetWorld();

	FVector aiLocation = ControllingPawn->GetActorLocation();
	float DetectTreesRadius = 10000.f;
	//��ü �ݰ�(10000������) �ȿ� tree ���͵� ����
	TArray <FOverlapResult> OverlapedTrees;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);
	bool bResult = World->OverlapMultiByChannel(
		OverlapedTrees,
		aiLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(DetectTreesRadius),
		CollisionQueryParam
	);

	std::vector<ATree*> trees;

	// ������Ʈ�� �����Ǹ�, tree���� �˻�
	if (bResult)
	{
		for (FOverlapResult it : OverlapedTrees)
		{
			ATree* tree = Cast<ATree>(it.GetActor());
			if (tree)
			{				
				trees.push_back(tree);
			}
		
		}
	}
	if (0 != trees.size()) {
		//����� ��
		ATree* t = GetClosestTree(aiLocation, trees);
		if (nullptr != t)
		{
			DrawDebugSphere(World, aiLocation, 2000.f, 16, FColor::Purple, false, 0.2f);
			DrawDebugPoint(World, t->GetActorLocation(), 10.0f, FColor::Cyan, false, 0.2f);
			DrawDebugLine(World, aiLocation, t->GetActorLocation(), FColor::Cyan, false, 0.2f);
			//closestTree �����忡 ����.
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAIController_Custom::TreePosKey, t);
		}
	}
	
	AAICharacter* ai = Cast<AAICharacter>(OwnerComp.GetAIOwner()->GetPawn());

	if (!ai->bAttacking)
		return EBTNodeResult::Succeeded;
	return EBTNodeResult::Failed;
}
