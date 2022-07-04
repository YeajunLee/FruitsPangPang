// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_Sword_Controller_Custom.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName AAI_Sword_Controller_Custom::SwordTargetKey(TEXT("SwordTarget"));

FName AAI_Sword_Controller_Custom::PunnetKey(TEXT("Punnet"));
FName AAI_Sword_Controller_Custom::PunnetItemKey(TEXT("PunnetItem"));



AAI_Sword_Controller_Custom::AAI_Sword_Controller_Custom()
	:SavedItemCode{-1}
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("BlackboardData'/Game/Character/AICharacter/SwordAI/BB_SwordAI.BB_SwordAI'"));
	
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("BehaviorTree'/Game/Character/AICharacter/SwordAI/BT_SwordAI.BT_SwordAI'"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}
}

void AAI_Sword_Controller_Custom::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (UseBlackboard(BBAsset, Blackboard))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Runtree"));
		RunBehaviorTree(BTAsset);

	}
}
