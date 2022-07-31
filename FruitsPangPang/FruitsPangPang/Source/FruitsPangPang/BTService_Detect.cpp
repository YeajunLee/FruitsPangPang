// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Detect.h"
#include "AICharacter.h"
#include "AIController_Custom.h"
#include "AI_Smart_Controller_Custom.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
//#include "MyCharacter.h"

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

	//2022-07-05
	AAICharacter* ai = Cast<AAICharacter>(ControllingPawn);

	auto AIController = Cast<AAIController_Custom>(ai->Controller);
	auto smartAIController = Cast<AAI_Smart_Controller_Custom>(ai->Controller);
	//

	UWorld* World = ControllingPawn->GetWorld();
	if (nullptr == World) return;

	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = 900.0f;

	//2022-07-16
	if (smartAIController)
	{
		DetectRadius = 1500.f;
	}

	// 600의 반지름을 가진 구체를 만들어서 오브젝트를 감지한다.
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

	//2022-07-14
	/*if (smartAIController)
	{
		if (ai->bIsDie)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAI_Smart_Controller_Custom::TargetKey, nullptr);
		}
	}*/

	// 오브젝트가 감지가 되면, 그 오브젝트가 Character인지 검사한다.
	if (bResult)
	{
		for (FOverlapResult OverlapResult : OverlapResults)
		{
			//AMyCharacter* myCharacter = Cast<AMyCharacter>(OverlapResult.GetActor());
			ABaseCharacter* newbie = Cast<ABaseCharacter>(OverlapResult.GetActor());

			if (nullptr != newbie)
			{
				if (AIController)
				{			
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAIController_Custom::TargetKey, newbie);

					// 디버깅 용.
					DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Purple, false, 0.2f);
					DrawDebugPoint(World, newbie->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);
					DrawDebugLine(World, ControllingPawn->GetActorLocation(), newbie->GetActorLocation(), FColor::Blue, false, 0.2f);
				}

				else if (smartAIController)
				{
					//2022-07-16
					auto oldman = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAI_Smart_Controller_Custom::TargetKey));
					if (nullptr != oldman)
					{
						if (oldman->bIsDie) {
							OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAI_Smart_Controller_Custom::TargetKey, nullptr);
							return;
						}
						if (oldman->hp > newbie->hp)
						{
							OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAI_Smart_Controller_Custom::TargetKey, newbie); // Character면, 블랙보드에 저장한다.
							// 디버깅 용.
							DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
							DrawDebugPoint(World, newbie->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);
							DrawDebugLine(World, ControllingPawn->GetActorLocation(), newbie->GetActorLocation(), FColor::Blue, false, 0.2f);
						}
						return;

					}
					//

					OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAIController_Custom::TargetKey, newbie);
					DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
					DrawDebugPoint(World, newbie->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);
					DrawDebugLine(World, ControllingPawn->GetActorLocation(), newbie->GetActorLocation(), FColor::Blue, false, 0.2f);
				}
				

				
				return;
			}
		}
	}
	else
	{
		if(AIController)
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAIController_Custom::TargetKey, nullptr);
		else if(smartAIController)
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(AAI_Smart_Controller_Custom::TargetKey, nullptr);
	}

	if (AIController)
		DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
	else if(smartAIController)
		DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Orange, false, 0.2f);

}