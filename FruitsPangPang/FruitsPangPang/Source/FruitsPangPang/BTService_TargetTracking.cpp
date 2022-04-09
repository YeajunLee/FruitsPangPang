// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_TargetTracking.h"
#include "AIController_Custom.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MyCharacter.h"
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

	auto Target = Cast<AMyCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAIController_Custom::TargetKey));
	if (Target)
	{
		FVector LookVector = Target->GetActorLocation() - ControllingPawn->GetActorLocation();

		FRotator LookRotator = LookVector.Rotation();
		OwnerComp.GetBlackboardComponent()->SetValueAsRotator(AAIController_Custom::TrackingTargetKey, LookRotator);

		DrawDebugLine(GetWorld(), ControllingPawn->GetActorLocation(), Target->GetActorLocation(), FColor::Yellow, false, 0.2f);

	}
}
