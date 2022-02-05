// Fill out your copyright notice in the Description page of Project Settings.


#include "AIController_Custom.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName AAIController_Custom::HomePosKey(TEXT("HomePos"));
const FName AAIController_Custom::PatrolPosKey(TEXT("PatrolPos"));
const FName AAIController_Custom::TargetKey(TEXT("Target"));

AAIController_Custom::AAIController_Custom()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("BlackboardData'/Game/Character/AICharacter/BB_AICharacter.BB_AICharacter'"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("BehaviorTree'/Game/Character/AICharacter/BT_AICharacter.BT_AICharacter'"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}
}

void AAIController_Custom::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (UseBlackboard(BBAsset, Blackboard))
	{
		RunBehaviorTree(BTAsset);

		//Blackboard->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
	}
}