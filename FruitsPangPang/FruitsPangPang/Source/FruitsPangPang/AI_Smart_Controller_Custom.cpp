// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_Smart_Controller_Custom.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName AAI_Smart_Controller_Custom::TargetKey(TEXT("Target"));

FName AAI_Smart_Controller_Custom::PunnetKey(TEXT("Punnet"));
FName AAI_Smart_Controller_Custom::PunnetItemKey(TEXT("PunnetItem"));

FName AAI_Smart_Controller_Custom::TreePosKey(TEXT("TreePos"));
FName AAI_Smart_Controller_Custom::AmountKey(TEXT("Amount"));
FName AAI_Smart_Controller_Custom::TrackingTargetKey(TEXT("TrackingTarget"));


AAI_Smart_Controller_Custom::AAI_Smart_Controller_Custom()
	:SavedItemCode{ -1 }
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("BlackboardData'/Game/Character/AICharacter/SmartAI/BB_SmartAI.BB_SmartAI'"));

	if(BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("BehaviorTree'/Game/Character/AICharacter/SmartAI/BT_SmartAI.BT_SmartAI'"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}

}

void AAI_Smart_Controller_Custom::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (UseBlackboard(BBAsset, Blackboard))
	{
		RunBehaviorTree(BTAsset);
	}
}
