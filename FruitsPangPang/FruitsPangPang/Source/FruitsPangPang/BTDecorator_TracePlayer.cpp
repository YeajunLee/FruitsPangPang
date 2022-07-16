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
	//Detect���� �ݰ� �ȿ� ���� target�� �����ϰ� attack ���������� ���ؼ���
	//���� Ʈ���̽��� �ؼ� ���� ���� ��(������ �տ� ���� ��) attack �ϵ��� �Ѵ�.

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // ��Ʈ ������ ������Ʈ ������.
	TArray<AActor*> IgnoreActors; // ������ ���͵�.
	//TArray<FHitResult> HitResult; // ��Ʈ ��� �� ���� ����.
	FHitResult HitResult;

	TEnumAsByte<EObjectTypeQuery> WorldStatic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic);
	TEnumAsByte<EObjectTypeQuery> WorldPawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);

	ObjectTypes.Add(WorldStatic);
	ObjectTypes.Add(WorldPawn);
	IgnoreActors.Add(ControllingPawn); //ignoreself(������ �Ķ����)�� �ִµ�?

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
