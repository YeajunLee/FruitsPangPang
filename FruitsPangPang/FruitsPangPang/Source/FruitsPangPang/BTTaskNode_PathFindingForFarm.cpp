// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_PathFindingForFarm.h"
#include "AIController_Custom.h"
#include "AI_Smart_Controller_Custom.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "AICharacter.h"
#include "MyCharacter.h"
#include "Tree.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/KismetSystemLibrary.h"



UBTTaskNode_PathFindingForFarm::UBTTaskNode_PathFindingForFarm()
{
	NodeName = TEXT("CPP_BTTPathFindingForFarm");
	//bNotifyTick = true;
}



EBTNodeResult::Type UBTTaskNode_PathFindingForFarm::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	//UObject* TreePos = OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAIController_Custom::TreePosKey);
	//UAIBlueprintHelperLibrary::SimpleMoveToActor(ControllingPawn->GetController(), static_cast<AActor*>(TreePos));

	UWorld* World = ControllingPawn->GetWorld();
	if (nullptr == World) { 
		return EBTNodeResult::Failed;
	}

	AAICharacter* mAI = Cast<AAICharacter>(ControllingPawn);

	//2022-07-05
	auto AIController = Cast<AAIController_Custom>(mAI->Controller);
	auto smartAIController = Cast<AAI_Smart_Controller_Custom>(mAI->Controller);

	if (mAI->bIsDie) {
		return EBTNodeResult::Failed;
	}
	//---------------------
	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = 10000.0f;

	// 900의 반지름을 가진 구체를 만들어서 오브젝트를 감지한다.
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);

	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_WorldStatic,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);

	std::vector<TreeInfo>& trees = mAI->trees;
	int& TargetTreeNum = mAI->TargetTreeNum;
	float& fTreeDistance = mAI->fTreeDistance;
	// 오브젝트가 감지가 되면, 그 오브젝트가 tree인지 검사한다.
	if (bResult)
	{
		for (FOverlapResult OverlapResult : OverlapResults)
		{
			ATree* Tree = Cast<ATree>(OverlapResult.GetActor());
			if (nullptr != Tree)
			{
				//trees의 생성자에 Tree를 직접 줘서, 복사,이동이 안일어나게 함.
				trees.emplace_back(Tree);
			}

		}
	}

	// --- 가장 가까운 나무 찾기,
	if (trees.size() <= 0)
		return EBTNodeResult::Failed;

	//Trees에 있는 객체가 아닌 Tree의 index를 꺼내주는건, 탐색에 실패했을 때, trees의 ignore값을 true로 바꿔주고
	//재 탐색해야하기 때문.
	//재사용을 위한 코드
   TargetTreeNum = -1;
   fTreeDistance = TNumericLimits<float>::Max();
	FVector mAILocation = mAI->GetActorLocation();

	for (int i = 0; i < trees.size(); ++i)
	{
		float distance = FVector::DistSquared(mAILocation, trees[i].mTree->GetActorLocation());
		//Ignored 상태인 tree는 탐색에 넣지 않는다.
		if (trees[i].bIgnored) continue;
		if (trees[i].mTree->CanHarvest)
		{
			if (distance < fTreeDistance)
			{
				fTreeDistance = distance;
				TargetTreeNum = i;
			}
		}
	}
	//재사용을 위해 다시 초기화.
	fTreeDistance = TNumericLimits<float>::Max();
	//--- 가장 가까운 나무 찾기 끝.
	// TargetTree = 가장 가까운 나무
	if (-1 == TargetTreeNum)
	{
		//patrol other spot
		//가까운곳에 나무가 아예 없다면 랜덤한 장소로 이동한다. (플레이어와 마주치는짓은 하지 않는다)
		//현재는 그냥 Failed를 반환
		UE_LOG(LogTemp, Error, TEXT("Ai Can't Find Tree"));
		trees.clear();
		return 	EBTNodeResult::Failed; 
		
	}
	//-- 내 자신에서부터 나무로부터 광선을 쏴서 이 사이에 캐릭터가 있는지 확인한다.
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // 히트 가능한 오브젝트 유형들.
	TArray<AActor*> IgnoreActors; // 무시할 액터들.
	FHitResult HitResult; // 히트 결과 값 받을 변수.

	//TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
	TEnumAsByte<EObjectTypeQuery> WorldPawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	//ObjectTypes.Add(WorldStatic);
	ObjectTypes.Add(WorldPawn);
	IgnoreActors.Add(ControllingPawn);

	while (1) {
		UKismetSystemLibrary::LineTraceSingleForObjects(
			mAI->GetWorld()
			, mAILocation, trees[TargetTreeNum].mTree->GetActorLocation()
			, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::ForOneFrame, HitResult, true);

		ABaseCharacter* BotherPlayer = Cast<ABaseCharacter>(HitResult.GetActor());
		//------------------------------
		if (nullptr != BotherPlayer)
		{
			//나무와 나 사이에 적이 있다는 얘기
			trees[TargetTreeNum].bIgnored = true;
			//다시 탐색한다.
			bool bCanNotFindTree = true;
			fTreeDistance = TNumericLimits<float>::Max();
			for (int i = 0; i < trees.size(); ++i)
			{
				float distance = FVector::DistSquared(mAILocation, trees[i].mTree->GetActorLocation());
				//Ignored 상태인 tree는 탐색에 넣지 않는다.
				if (trees[i].bIgnored) continue;
				if (trees[i].mTree->CanHarvest)
				{
					if (distance < fTreeDistance)
					{
						fTreeDistance = distance;
						TargetTreeNum = i;
						bCanNotFindTree = false;
					}
				}
			}

			if (bCanNotFindTree)
			{
				//반복문을 다 돌았는데 (모든 나무탐색을 했는데)
				// 내가 갈 나무가 아무것도 존재하지 않다면
				//랜덤무브를 해줘야함. 아직 코딩하진않겠음.
				//현재는 그냥 Failed처리
				trees.clear();
				fTreeDistance = TNumericLimits<float>::Max();
				return EBTNodeResult::Failed;
			}
			//재사용을 위한 초기화.
			fTreeDistance = TNumericLimits<float>::Max();
		}
		else {
			//방해하는 적이 없다면 탈출해서 나무로 달려간다.

			if (AIController)
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAIController_Custom::TreePosKey, trees[TargetTreeNum].mTree);
				FVector goal = FVector(trees[TargetTreeNum].mTree->GetActorLocation());
				//goal.Y += 70;
				//UAIBlueprintHelperLibrary::SimpleMoveToLocation(ControllingPawn->GetController(), goal);
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(ControllingPawn->GetController(), trees[TargetTreeNum].mTree->GetActorLocation());
			}
			else if (smartAIController)
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAI_Smart_Controller_Custom::TreePosKey, trees[TargetTreeNum].mTree);
				FVector goal = FVector(trees[TargetTreeNum].mTree->GetActorLocation());
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(ControllingPawn->GetController(), trees[TargetTreeNum].mTree->GetActorLocation());
			}
			
			break;
		}
	}


	if (mAI->bIsDie) {
		trees.clear(); 
		return EBTNodeResult::Failed;
	}

	if (mAI->bIsUndertheTree)
	{
		trees.clear();
		//UE_LOG(LogTemp, Error, TEXT("YesUndertheTree"));

		return EBTNodeResult::Succeeded;
	}

	
	return EBTNodeResult::Failed;
}

//void UBTTaskNode_PathFindingForFarm::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
//{
//	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
//
//	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
//	AAICharacter* mAI = Cast<AAICharacter>(ControllingPawn);
//
//	std::vector<TreeInfo>& trees = mAI->trees;
//	int& TargetTreeNum = mAI->TargetTreeNum;
//	float& fTreeDistance = mAI->fTreeDistance;
//	fTreeDistance = TNumericLimits<float>::Max();
//
//	FVector mAILocation = mAI->GetActorLocation();
//
//	//-- 내 자신에서부터 나무로부터 광선을 쏴서 이 사이에 캐릭터가 있는지 확인한다.
//	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // 히트 가능한 오브젝트 유형들.
//	TArray<AActor*> IgnoreActors; // 무시할 액터들.
//	FHitResult HitResult; // 히트 결과 값 받을 변수.
//
//	//TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
//	TEnumAsByte<EObjectTypeQuery> WorldPawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
//	//ObjectTypes.Add(WorldStatic);
//	ObjectTypes.Add(WorldPawn);
//	IgnoreActors.Add(ControllingPawn);
//	while (1) {
//		UKismetSystemLibrary::LineTraceSingleForObjects(
//			mAI->GetWorld()
//			, mAILocation, trees[TargetTreeNum].mTree->GetActorLocation()
//			, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::ForOneFrame, HitResult, true);
//
//		ABaseCharacter* BotherPlayer = Cast<ABaseCharacter>(HitResult.GetActor());
//		//------------------------------
//		if (nullptr != BotherPlayer)
//		{
//			//나무와 나 사이에 적이 있다는 얘기
//			trees[TargetTreeNum].bIgnored = true;
//			//다시 탐색한다.
//			bool bCanNotFindTree = true;
//			fTreeDistance = TNumericLimits<float>::Max();
//			for (int i = 0; i < trees.size(); ++i)
//			{
//				float distance = FVector::DistSquared(mAILocation, trees[i].mTree->GetActorLocation());
//				//Ignored 상태인 tree는 탐색에 넣지 않는다.
//				if (trees[i].bIgnored) continue;
//				if (trees[i].mTree->CanHarvest)
//				{
//					if (distance < fTreeDistance)
//					{
//						fTreeDistance = distance;
//						TargetTreeNum = i;
//						bCanNotFindTree = false;
//					}
//				}
//			}
//
//			if (bCanNotFindTree)
//			{
//				//반복문을 다 돌았는데 (모든 나무탐색을 했는데)
//				// 내가 갈 나무가 아무것도 존재하지 않다면
//				//랜덤무브를 해줘야함. 아직 코딩하진않겠음.
//				//현재는 그냥 Failed처리
//				trees.clear();
//				FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
//				fTreeDistance = TNumericLimits<float>::Max();
//				return;
//			}
//			//재사용을 위한 초기화.
//			fTreeDistance = TNumericLimits<float>::Max();
//		}
//		else {
//			//방해하는 적이 없다면 탈출해서 나무로 달려간다.
//			OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAIController_Custom::TreePosKey, trees[TargetTreeNum].mTree);
//
//			FVector goal = FVector(trees[TargetTreeNum].mTree->GetActorLocation());
//			goal.Y += 150;
//			//UAIBlueprintHelperLibrary::SimpleMoveToLocation(ControllingPawn->GetController(), goal);
//			UAIBlueprintHelperLibrary::SimpleMoveToLocation(ControllingPawn->GetController(), trees[TargetTreeNum].mTree->GetActorLocation());
//			//UAIBlueprintHelperLibrary::SimpleMoveToActor(ControllingPawn->GetController(), static_cast<AActor*>(trees[TargetTreeNum].mTree));
//			break;
//		}
//	}
//
//	if (mAI->bIsUndertheTree)
//	{
//		trees.clear();
//		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
//	}
//
//}
//
//void UBTTaskNode_PathFindingForFarm::FindTree()
//{
//}