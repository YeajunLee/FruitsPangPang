// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "AICharacter.h"
#include "AIController_Custom.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "MyCharacter.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("CPP_BTSDetect");
	Interval = 1.0f;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return;

	UWorld* World = ControllingPawn->GetWorld();
	if (nullptr == World) return;

	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = 600.0f;

	// 600의 반지름을 가진 구체를 만들어서 오브젝트를 감지한다.
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);

	// 오브젝트가 감지가 되면, 그 오브젝트가 Character인지 검사한다.
	if (bResult)
	{
		for (FOverlapResult OverlapResult : OverlapResults)
		{
			AMyCharacter* myCharacter = Cast<AMyCharacter>(OverlapResult.GetActor());

			if (nullptr != myCharacter)
			{
				// Character면, 블랙보드에 저장한다.
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAIController_Custom::TargetKey, myCharacter);

				// 디버깅 용.
				DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
				DrawDebugPoint(World, myCharacter->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);
				DrawDebugLine(World, ControllingPawn->GetActorLocation(), myCharacter->GetActorLocation(), FColor::Blue, false, 0.2f);
				return;
			}
		}
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAIController_Custom::TargetKey, nullptr);
	}

	DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
}