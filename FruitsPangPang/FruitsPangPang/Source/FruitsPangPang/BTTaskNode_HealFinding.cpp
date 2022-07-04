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

	// ������Ʈ�� ������ �Ǹ�, �� ������Ʈ�� HealSpawner���� �˻��Ѵ�.
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

	// ���� ����� (CanHarvest)HealSpawner ã�� ------------------------------------------
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

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // ��Ʈ ������ ������Ʈ ������
	TArray<AActor*> IgnoreActors; // ������ ���͵�
	FHitResult HitResult; // ��Ʈ ��� �� ���� ����

	TEnumAsByte<EObjectTypeQuery> WorldPawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	ObjectTypes.Add(WorldPawn);
	IgnoreActors.Add(ControllingPawn);

	while (true) {
		UKismetSystemLibrary::LineTraceSingleForObjects(
			ai->GetWorld()
			, source, healspawners[TargetHealSpanwerIndex].mHealSpawner->GetActorLocation()
			, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::ForOneFrame, HitResult, true);

		// �Ĺ��Ϸ� ���� �߿� Linetracing�� �ؼ� �÷��̾� ĳ���Ͱ� �տ� �ִٸ�
		ABaseCharacter* BotherPlayer = Cast<ABaseCharacter>(HitResult.GetActor());
		if (BotherPlayer)
		{
			// �ش� punnet ����
			healspawners[TargetHealSpanwerIndex].bIgnored = true;

			// ��Ž��
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
				//punnet�� ������ patrol, ������ �ϴ� failed
				healspawners.clear();
				return 	EBTNodeResult::Failed;
			}
			ClosestHealSpawnerDistance = TNumericLimits<float>::Max();
		}
		else
		{
			// �����ϴ� �÷��̾ ���ٸ�
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
