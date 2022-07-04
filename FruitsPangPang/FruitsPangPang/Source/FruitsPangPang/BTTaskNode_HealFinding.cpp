// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_HealFinding.h"
#include "AIController_Custom.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "HealSpawner.h"
#include "AICharacter.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UBTTaskNode_HealFinding::UBTTaskNode_HealFinding()
{
	NodeName = TEXT("HealFinding");

}

EBTNodeResult::Type UBTTaskNode_HealFinding::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	UWorld* World = ControllingPawn->GetWorld();
	if (nullptr == World) {
		return EBTNodeResult::Failed;
	}

	auto ai = Cast<AAICharacter>(ControllingPawn);
	if (ai->bIsDie) {
		return EBTNodeResult::Failed;
	}

	FVector source = ControllingPawn->GetActorLocation();
	float DetectRadius = 25000.f;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);

	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		source,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);

	std::vector<HealSpawnerInfo> healspawners = ai->heals;
	int TargetHealSpanwerIndex = ai->TargetHealSpawnerIndex;
	float ClosestHealSpawnerDistance = ai->ClosestHealSpawnerDistance;

	// 오브젝트가 감지가 되면, 그 오브젝트가 HealSpawner인지 검사한다.
	if (bResult)
	{
		for (FOverlapResult OverlapResult : OverlapResults)
		{
			AHealSpawner* healspawner = Cast<AHealSpawner>(OverlapResult.GetActor());
			if (nullptr != healspawner)
			{
				//UE_LOG(LogTemp, Log, TEXT("Punnet Find!!"));
				healspawners.emplace_back(healspawner);
			}
		}
	}

	// 가장 가까운 (CanHarvest)HealSpawner 찾기 ------------------------------------------
	if (healspawners.size() <= 0)
		return EBTNodeResult::Failed;

	TargetHealSpanwerIndex = -1;
	ClosestHealSpawnerDistance = TNumericLimits<float>::Max();

	for (int i = 0; i < healspawners.size(); ++i)
	{
		if (healspawners[i].mHealSpawner->CanHarvest == true)
		{
			float distance = FVector::DistSquared(source, healspawners[i].mHealSpawner->GetActorLocation());

			if (healspawners[i].bIgnored) continue;
			if (distance < ClosestHealSpawnerDistance)
			{
				ClosestHealSpawnerDistance = distance;
				TargetHealSpanwerIndex = i;
			}
		}
	
	}
	ClosestHealSpawnerDistance = TNumericLimits<float>::Max();

	if (-1 == TargetHealSpanwerIndex)
	{
		healspawners.clear();
		return 	EBTNodeResult::Failed;
	}

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // 히트 가능한 오브젝트 유형들
	TArray<AActor*> IgnoreActors; // 무시할 엑터들
	FHitResult HitResult; // 히트 결과 값 받을 변수

	TEnumAsByte<EObjectTypeQuery> WorldPawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	ObjectTypes.Add(WorldPawn);
	IgnoreActors.Add(ControllingPawn);

	while (true) {
		UKismetSystemLibrary::LineTraceSingleForObjects(
			ai->GetWorld()
			, source, healspawners[TargetHealSpanwerIndex].mHealSpawner->GetActorLocation()
			, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::ForOneFrame, HitResult, true);

		// 파밍하러 가는 중에 Linetracing을 해서 플레이어 캐릭터가 앞에 있다면
		ABaseCharacter* BotherPlayer = Cast<ABaseCharacter>(HitResult.GetActor());
		if (BotherPlayer)
		{
			// 해당 punnet 무시
			healspawners[TargetHealSpanwerIndex].bIgnored = true;

			// 재탐색
			TargetHealSpanwerIndex = -1;
			ClosestHealSpawnerDistance = TNumericLimits<float>::Max();
			for (int i = 0; i < healspawners.size(); ++i)
			{
				float distance = FVector::DistSquared(source, healspawners[i].mHealSpawner->GetActorLocation());

				if (healspawners[i].bIgnored) continue;
				if (distance < ClosestHealSpawnerDistance)
				{
					ClosestHealSpawnerDistance = distance;
					TargetHealSpanwerIndex = i;
				}
			}

			if (-1 == TargetHealSpanwerIndex)
			{
				//punnet이 없으면 patrol, 지금은 일단 failed
				healspawners.clear();
				return 	EBTNodeResult::Failed;
			}
			ClosestHealSpawnerDistance = TNumericLimits<float>::Max();
		}
		else
		{
			// 방해하는 플레이어가 없다면
			UAIBlueprintHelperLibrary::SimpleMoveToActor(ControllingPawn->GetController(), healspawners[TargetHealSpanwerIndex].mHealSpawner);
			break;
		}
	}

	if (ai->bIsDie) {
		healspawners.clear();
		return EBTNodeResult::Failed;
	}
	if (ai->bIsUndertheHealSpawner)
	{
		healspawners.clear();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;

}
