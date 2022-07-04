// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_TargetTracking.h"
#include "AICharacter.h"
#include "AIController_Custom.h"
#include "AI_Smart_Controller_Custom.h"
#include "BehaviorTree/BlackboardComponent.h"
//#include "MyCharacter.h"
#include "DrawDebugHelpers.h"


UBTService_TargetTracking::UBTService_TargetTracking()
{
	NodeName = TEXT("CPP_BTSTargetTracking");
}

void UBTService_TargetTracking::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return;

	//2022-07-05
	AAICharacter* ai = Cast<AAICharacter>(ControllingPawn);
	auto AIController = Cast<AAIController_Custom>(ai->Controller);
	auto smartAIController = Cast<AAI_Smart_Controller_Custom>(ai->Controller);

	if (AIController)
	{
		auto Target = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAIController_Custom::TargetKey));
		if (Target)
		{
			FVector LookVector = Target->GetActorLocation() - ControllingPawn->GetActorLocation();

			FRotator LookRotator = LookVector.Rotation();
			OwnerComp.GetBlackboardComponent()->SetValueAsRotator(AAIController_Custom::TrackingTargetKey, LookRotator);

			DrawDebugLine(GetWorld(), ControllingPawn->GetActorLocation(), Target->GetActorLocation(), FColor::Yellow, false, 0.2f);
		}
	}
	else if (smartAIController)
	{
		auto Target = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAI_Smart_Controller_Custom::TargetKey));
		if (Target)
		{
			FVector LookVector = Target->GetActorLocation() - ControllingPawn->GetActorLocation();

			FRotator LookRotator = LookVector.Rotation();
			OwnerComp.GetBlackboardComponent()->SetValueAsRotator(AAI_Smart_Controller_Custom::TrackingTargetKey, LookRotator);

			DrawDebugLine(GetWorld(), ControllingPawn->GetActorLocation(), Target->GetActorLocation(), FColor::Yellow, false, 0.2f);
		}
	}
	
}
