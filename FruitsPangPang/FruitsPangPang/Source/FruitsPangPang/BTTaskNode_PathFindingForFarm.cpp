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

	// 900�� �������� ���� ��ü�� ���� ������Ʈ�� �����Ѵ�.
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
	// ������Ʈ�� ������ �Ǹ�, �� ������Ʈ�� tree���� �˻��Ѵ�.
	if (bResult)
	{
		for (FOverlapResult OverlapResult : OverlapResults)
		{
			ATree* Tree = Cast<ATree>(OverlapResult.GetActor());
			if (nullptr != Tree)
			{
				//trees�� �����ڿ� Tree�� ���� �༭, ����,�̵��� ���Ͼ�� ��.
				trees.emplace_back(Tree);
			}

		}
	}

	// --- ���� ����� ���� ã��,
	if (trees.size() <= 0)
		return EBTNodeResult::Failed;

	//Trees�� �ִ� ��ü�� �ƴ� Tree�� index�� �����ִ°�, Ž���� �������� ��, trees�� ignore���� true�� �ٲ��ְ�
	//�� Ž���ؾ��ϱ� ����.
	//������ ���� �ڵ�
   TargetTreeNum = -1;
   fTreeDistance = TNumericLimits<float>::Max();
	FVector mAILocation = mAI->GetActorLocation();

	for (int i = 0; i < trees.size(); ++i)
	{
		float distance = FVector::DistSquared(mAILocation, trees[i].mTree->GetActorLocation());
		//Ignored ������ tree�� Ž���� ���� �ʴ´�.
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
	//������ ���� �ٽ� �ʱ�ȭ.
	fTreeDistance = TNumericLimits<float>::Max();
	//--- ���� ����� ���� ã�� ��.
	// TargetTree = ���� ����� ����
	if (-1 == TargetTreeNum)
	{
		//patrol other spot
		//�������� ������ �ƿ� ���ٸ� ������ ��ҷ� �̵��Ѵ�. (�÷��̾�� ����ġ������ ���� �ʴ´�)
		//����� �׳� Failed�� ��ȯ
		UE_LOG(LogTemp, Error, TEXT("Ai Can't Find Tree"));
		trees.clear();
		return 	EBTNodeResult::Failed; 
		
	}
	//-- �� �ڽſ������� �����κ��� ������ ���� �� ���̿� ĳ���Ͱ� �ִ��� Ȯ���Ѵ�.
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // ��Ʈ ������ ������Ʈ ������.
	TArray<AActor*> IgnoreActors; // ������ ���͵�.
	FHitResult HitResult; // ��Ʈ ��� �� ���� ����.

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
			//������ �� ���̿� ���� �ִٴ� ���
			trees[TargetTreeNum].bIgnored = true;
			//�ٽ� Ž���Ѵ�.
			bool bCanNotFindTree = true;
			fTreeDistance = TNumericLimits<float>::Max();
			for (int i = 0; i < trees.size(); ++i)
			{
				float distance = FVector::DistSquared(mAILocation, trees[i].mTree->GetActorLocation());
				//Ignored ������ tree�� Ž���� ���� �ʴ´�.
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
				//�ݺ����� �� ���Ҵµ� (��� ����Ž���� �ߴµ�)
				// ���� �� ������ �ƹ��͵� �������� �ʴٸ�
				//�������긦 �������. ���� �ڵ������ʰ���.
				//����� �׳� Failedó��
				trees.clear();
				fTreeDistance = TNumericLimits<float>::Max();
				return EBTNodeResult::Failed;
			}
			//������ ���� �ʱ�ȭ.
			fTreeDistance = TNumericLimits<float>::Max();
		}
		else {
			//�����ϴ� ���� ���ٸ� Ż���ؼ� ������ �޷�����.

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
//	//-- �� �ڽſ������� �����κ��� ������ ���� �� ���̿� ĳ���Ͱ� �ִ��� Ȯ���Ѵ�.
//	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // ��Ʈ ������ ������Ʈ ������.
//	TArray<AActor*> IgnoreActors; // ������ ���͵�.
//	FHitResult HitResult; // ��Ʈ ��� �� ���� ����.
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
//			//������ �� ���̿� ���� �ִٴ� ���
//			trees[TargetTreeNum].bIgnored = true;
//			//�ٽ� Ž���Ѵ�.
//			bool bCanNotFindTree = true;
//			fTreeDistance = TNumericLimits<float>::Max();
//			for (int i = 0; i < trees.size(); ++i)
//			{
//				float distance = FVector::DistSquared(mAILocation, trees[i].mTree->GetActorLocation());
//				//Ignored ������ tree�� Ž���� ���� �ʴ´�.
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
//				//�ݺ����� �� ���Ҵµ� (��� ����Ž���� �ߴµ�)
//				// ���� �� ������ �ƹ��͵� �������� �ʴٸ�
//				//�������긦 �������. ���� �ڵ������ʰ���.
//				//����� �׳� Failedó��
//				trees.clear();
//				FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
//				fTreeDistance = TNumericLimits<float>::Max();
//				return;
//			}
//			//������ ���� �ʱ�ȭ.
//			fTreeDistance = TNumericLimits<float>::Max();
//		}
//		else {
//			//�����ϴ� ���� ���ٸ� Ż���ؼ� ������ �޷�����.
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