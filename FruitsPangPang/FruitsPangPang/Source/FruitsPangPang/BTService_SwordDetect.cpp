// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SwordDetect.h"
//#include "AI_SwordCharacter.h"
#include "AICharacter.h"
#include "AI_Sword_Controller_Custom.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
//#include "MyCharacter.h"

UBTService_SwordDetect::UBTService_SwordDetect()
{
	NodeName = TEXT("SwordDetect");
	Interval = 1.0f;
}

void UBTService_SwordDetect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return;

	UWorld* World = ControllingPawn->GetWorld();
	if (nullptr == World) return;

	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = 5000.0f;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_Pawn,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);

	if (bResult)
	{
		for (FOverlapResult OverlapResult : OverlapResults)
		{
			//AMyCharacter* myCharacter = Cast<AMyCharacter>(OverlapResult.GetActor());
			auto Character = Cast<ABaseCharacter>(OverlapResult.GetActor());


			if (nullptr != Character)
			{
				//UE_LOG(LogTemp, Log, TEXT("find enemy!!(sword_AI)"));

				// Character면, 블랙보드에 저장한다.
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAI_Sword_Controller_Custom::SwordTargetKey, Character);

				// 디버깅 용.
				DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
				DrawDebugPoint(World, Character->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);
				DrawDebugLine(World, ControllingPawn->GetActorLocation(), Character->GetActorLocation(), FColor::Blue, false, 0.2f);
				return;
			}
		}
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAI_Sword_Controller_Custom::SwordTargetKey, nullptr);
	}

	DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);

}
