// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_TracePlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AICharacter.h"
#include "AIController_Custom.h"
#include "AI_Smart_Controller_Custom.h"
#include "Kismet/KismetSystemLibrary.h"


UBTDecorator_TracePlayer::UBTDecorator_TracePlayer()
{
	NodeName = TEXT("TracePlayer");

}

bool UBTDecorator_TracePlayer::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	auto ai = Cast<AAICharacter>(ControllingPawn);

	FVector aiLocation = ai->GetActorLocation();

	auto AIController = Cast<AAIController_Custom>(ai->Controller);
	auto smartAIController = Cast<AAI_Smart_Controller_Custom>(ai->Controller);

	ABaseCharacter* Target = nullptr;
	if (AIController)
		Target = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAIController_Custom::TargetKey));
	else if (smartAIController)
		Target = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAI_Smart_Controller_Custom::TargetKey));

	if (nullptr == Target) return false;

	//2022-07-15
	//Detect에서 반경 안에 들어온 target을 감지하고 attack 가능한지에 대해서는
	//레이 트레이싱을 해서 벽이 없을 때(상대방이 앞에 있을 때) attack 하도록 한다.

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // 히트 가능한 오브젝트 유형들.
	TArray<AActor*> IgnoreActors; // 무시할 액터들.
	//TArray<FHitResult> HitResult; // 히트 결과 값 받을 변수.
	FHitResult HitResult;

	TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
	TEnumAsByte<EObjectTypeQuery> WorldPawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);

	ObjectTypes.Add(WorldStatic);
	ObjectTypes.Add(WorldPawn);
	IgnoreActors.Add(ControllingPawn); //ignoreself(마지막 파라미터)가 있는데?

	UKismetSystemLibrary::LineTraceSingleForObjects(
		ai->GetWorld()
		, aiLocation, Target->GetActorLocation()
		, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::ForDuration, HitResult, true
		, FLinearColor::Gray, FLinearColor::Gray);


	//for (const auto& hit : HitResult) 
	//{
	//	UE_LOG(LogTemp, Log, TEXT("%s"), *UKismetSystemLibrary::GetDisplayName(hit.GetActor()));
	//	if (hit.GetActor() != ai)
	//		bOnlyPlayer = false;
	//}
	//ABaseCharacter* player = nullptr;
	ABaseCharacter* player = Cast<ABaseCharacter>(HitResult.GetActor());

	//UE_LOG(LogTemp, Log, TEXT("%s"), *UKismetSystemLibrary::GetDisplayName(HitResult.GetActor()));
	//UE_LOG(LogTemp, Log, TEXT("%s"), bOnlyPlayer? TEXT("true") : TEXT("false"));

	bool bResult = (HitResult.GetActor() == player);

	return bResult;
}
