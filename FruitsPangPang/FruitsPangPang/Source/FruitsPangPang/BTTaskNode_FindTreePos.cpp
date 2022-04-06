// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_FindTreePos.h"
#include "AIController_Custom.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tree.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "DrawDebugHelpers.h"


UBTTaskNode_FindTreePos::UBTTaskNode_FindTreePos()
{
	NodeName = TEXT("CPP_BTTFindTreePos");

	currentClosestDistance = TNumericLimits<float>::Max();
}

AActor* UBTTaskNode_FindTreePos::GetClosestActor(FVector sourceLocation, TArray<AActor*> actors)
{
	if (actors.Num() <= 0)
		return nullptr;

	AActor* closestActor = actors[0];
	//static float 

	for (int i = 0; i < actors.Num(); ++i)
	{
		float distance = FVector::DistSquared(sourceLocation, actors[i]->GetActorLocation());
		if (distance < currentClosestDistance)
		{
			currentClosestDistance = distance;
			closestActor = actors[i];

			//UE_LOG(LogTemp, Warning, TEXT("%s"), *closestActor->GetActorLocation().ToString());

		}
	}

	return closestActor;
}

EBTNodeResult::Type UBTTaskNode_FindTreePos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	UWorld* World = ControllingPawn->GetWorld();

	FVector aiLocation = ControllingPawn->GetActorLocation();
	float DetectTreesRadius = 2000.f;
	//구체 반경(2000반지름) 안에 tree 엑터들 감지
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

	//static float distance_ClosestTree_AI = TNumericLimits<float>::Max();

	TArray<AActor*> trees;

	// 오브젝트가 감지되면, tree인지 검사
	if (bResult)
	{
		for (FOverlapResult it : OverlapedTrees)
		{
			ATree* tree = Cast<ATree>(it.GetActor());

			if (tree)
			{
				trees.Add(tree);
			}
			
		}
	}
	if (0 != trees.Num()) {
		//디버깅 용
		DrawDebugSphere(World, aiLocation, 2000.f, 16, FColor::Purple, false, 0.2f);
		DrawDebugPoint(World, GetClosestActor(aiLocation, trees)->GetActorLocation(), 10.0f, FColor::Cyan, false, 0.2f);
		DrawDebugLine(World, aiLocation, GetClosestActor(aiLocation, trees)->GetActorLocation(), FColor::Cyan, false, 0.2f);

		//closestTree 블랙보드에 저장.
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AAIController_Custom::TreePosKey, GetClosestActor(aiLocation, trees)->GetActorLocation());
	}

	/*else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAIController_Custom::TargetTreeKey, nullptr);
	}*/

	/*AActor* trees = UGameplayStatics::GetActorOfClass(GetWorld(), ATree::StaticClass());
	FVector TreePos;
	TreePos = trees->GetActorLocation();

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(AAIController_Custom::TreePosKey, TreePos);*/

	//UAIBlueprintHelperLibrary::SimpleMoveToLocation(OwnerComp.GetAIOwner()->GetPawn()->GetController(), TreePos);
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *OwnerComp.GetBlackboardComponent()->GetValueAsVector(AAIController_Custom::TreePosKey).ToString());

	return EBTNodeResult::Succeeded;
}
