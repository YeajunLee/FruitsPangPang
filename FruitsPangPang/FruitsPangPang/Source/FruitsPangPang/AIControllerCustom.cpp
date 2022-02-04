// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerCustom.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName AAIControllerCustom::HomePosKey(TEXT("HomePos"));
const FName AAIControllerCustom::PatrolPosKey(TEXT("PatrolPos"));
const FName AAIControllerCustom::TargetKey(TEXT("Target"));

AAIControllerCustom::AAIControllerCustom()
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

void AAIControllerCustom::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (UseBlackboard(BBAsset, Blackboard))
	{
		RunBehaviorTree(BTAsset);

		//Blackboard->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
	}
}



