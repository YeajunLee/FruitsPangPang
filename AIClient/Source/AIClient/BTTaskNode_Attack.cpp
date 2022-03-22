// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_Attack.h"
#include "AICharacter.h"
#include "AIController_Custom.h"
#include "Inventory.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTaskNode_Attack::UBTTaskNode_Attack()
{
    NodeName = TEXT("CPP_BTTAttack");
    bNotifyTick = true; // 틱 기능 활성화?
    IsAttacking = false;
}

EBTNodeResult::Type UBTTaskNode_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    auto AICharacter = Cast<AAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == AICharacter)
        return EBTNodeResult::Failed;

    //update KeyAmount(BTS_subtomato before)--------------------------------------------------------
    int FruitAmount = AICharacter->mInventory->mSlots[AICharacter->SelectedHotKeySlotNum].Amount;
    OwnerComp.GetBlackboardComponent()->SetValueAsInt(AAIController_Custom::AmountKey, FruitAmount);
    //----------------------------------------------------------------------------------------------

    AICharacter->Attack();
    IsAttacking = true;
    AICharacter->OnAttackEnd.AddLambda([this]()->void { IsAttacking = false; });



    return EBTNodeResult::InProgress;
    // 공격 task는 공격 애니메이션이 끝날 때까지 대기해야 하는 지연 task이므로
    // ExecuteTask의 결과 값을 InProgress로 반환하고, 공격이 끝났을 때 task가 끝났다고 알려주어야 함.
    // 이를 알려주는 함수가 FinishLatentTask. 이 함수를 호출하지 않으면 행동트리 시스템은 현재 task에 계속 머물어 있는다.
    // 그래서 호출 할 수 있도록 노드의 tick 기능을 활성화하고 조건을 파악한 후 task 종료 명형을 내려주어야 함.
}

void UBTTaskNode_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    auto AICharacter = Cast<AAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == AICharacter)
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

    if (0 == AICharacter->mInventory->mSlots[AICharacter->SelectedHotKeySlotNum].Amount)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
    }

    if (!IsAttacking)
    {
        //UE_LOG(LogTemp, Warning, TEXT("Finish!"));    

        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
    
}
