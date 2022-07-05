// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_PunnetFinding.h"
#include "AI_Sword_Controller_Custom.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
//#include "AI_SwordCharacter.h"
#include "AICharacter.h"
//#include "MyCharacter.h"
#include "Punnet.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UBTTask_PunnetFinding::UBTTask_PunnetFinding()
{
	NodeName = TEXT("PunnetFinding");
	//bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_PunnetFinding::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	/*UObject* punnetObj = OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAI_Sword_Controller_Custom::PunnetKey);
	UAIBlueprintHelperLibrary::SimpleMoveToActor(ControllingPawn->GetController(), static_cast<AActor*>(punnetObj));*/

	UWorld* World = ControllingPawn->GetWorld();
	if (nullptr == World) {
		return EBTNodeResult::Failed;
	}

	auto swordAI = Cast<AAICharacter>(ControllingPawn);
	if (swordAI->bIsDie) {
		return EBTNodeResult::Failed;
	}

	FVector source = ControllingPawn->GetActorLocation();
	float DetectRadius = 20000.f;

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

	std::vector<PunnetInfo> punnets = swordAI->punnets;
	int TargetPunnetIndex = swordAI->TargetPunnetIndex;
	float ClosestPunnetDistance = swordAI->ClosestPunnetDistance;

	// ������Ʈ�� ������ �Ǹ�, �� ������Ʈ�� punnet���� �˻��Ѵ�.
	if (bResult)
	{
		for (FOverlapResult OverlapResult : OverlapResults)
		{
			APunnet* punnet = Cast<APunnet>(OverlapResult.GetActor());
			if (nullptr != punnet)
			{
				//UE_LOG(LogTemp, Log, TEXT("Punnet Find!!"));
				punnets.emplace_back(punnet);
			}
		}
	}

	// ���� ����� punnet ã�� ------------------------------------------
	if(punnets.size() <= 0)
		return EBTNodeResult::Failed;

	TargetPunnetIndex = -1;
	ClosestPunnetDistance = TNumericLimits<float>::Max();
	
	for (int i = 0; i < punnets.size(); ++i)
	{
		float distance = FVector::DistSquared(source, punnets[i].mPunnet->GetActorLocation());

		if (punnets[i].bIgnored) continue;
		/*if (punnets[i].mPunnet->CanHarvest)
		{			
		}*/
		if (distance < ClosestPunnetDistance)
		{
			ClosestPunnetDistance = distance;
			TargetPunnetIndex = i;
		}
	}

	ClosestPunnetDistance = TNumericLimits<float>::Max();

	if (-1 == TargetPunnetIndex) 
	{
		//punnet�� ������ patrol, ������ �ϴ� failed
		punnets.clear();
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
			swordAI->GetWorld()
			, source, punnets[TargetPunnetIndex].mPunnet->GetActorLocation()
			, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::ForOneFrame, HitResult, true);

		// �Ĺ��Ϸ� ���� �߿� Linetracing�� �ؼ� �÷��̾� ĳ���Ͱ� �տ� �ִٸ�
		ABaseCharacter* BotherPlayer = Cast<ABaseCharacter>(HitResult.GetActor());
		if (BotherPlayer)
		{
			// �ش� punnet ����
			punnets[TargetPunnetIndex].bIgnored = true;

			// ��Ž��
			TargetPunnetIndex = -1;
			ClosestPunnetDistance = TNumericLimits<float>::Max();
			for (int i = 0; i < punnets.size(); ++i)
			{
				float distance = FVector::DistSquared(source, punnets[i].mPunnet->GetActorLocation());

				if (punnets[i].bIgnored) continue;
				/*if (punnets[i].mPunnet->CanHarvest)
				{			
				}*/
				if (distance < ClosestPunnetDistance)
				{
					ClosestPunnetDistance = distance;
					TargetPunnetIndex = i;
				}
			}

			if (-1 == TargetPunnetIndex)
			{
				//punnet�� ������ patrol, ������ �ϴ� failed
				punnets.clear();
				return 	EBTNodeResult::Failed;
			}
			ClosestPunnetDistance = TNumericLimits<float>::Max();
		}
		else
		{
			// �����ϴ� �÷��̾ ���ٸ� GoToTheTree(Punnet)
			//OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAI_Sword_Controller_Custom::PunnetKey, punnets[TargetPunnetIndex].mPunnet);
			//UE_LOG(LogTemp, Warning, TEXT("%s"), punnets[TargetPunnetIndex].mPunnet);
			UAIBlueprintHelperLibrary::SimpleMoveToActor(ControllingPawn->GetController(), punnets[TargetPunnetIndex].mPunnet);
			break;
		}
	}

	if (swordAI->bIsDie) {
		punnets.clear();
		return EBTNodeResult::Failed;
	}
	if (swordAI->bIsUnderthePunnet)
	{
		punnets.clear();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}

void UBTTask_PunnetFinding::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
}
