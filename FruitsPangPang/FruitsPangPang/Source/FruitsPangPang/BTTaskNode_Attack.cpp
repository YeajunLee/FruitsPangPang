// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_Attack.h"
#include "AICharacter.h"
#include "AIController_Custom.h"
#include "AI_Smart_Controller_Custom.h"
#include "Inventory.h"
#include "BehaviorTree/BlackboardComponent.h"
//#include "MyCharacter.h"

UBTTaskNode_Attack::UBTTaskNode_Attack()
{
    NodeName = TEXT("CPP_BTTAttack");
    bNotifyTick = true; // ƽ ��� Ȱ��ȭ?
    IsAttacking = false;
}

EBTNodeResult::Type UBTTaskNode_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    auto AICharacter = Cast<AAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == AICharacter)
        return EBTNodeResult::Failed;

    //2022-07-05
    auto AIController = Cast<AAIController_Custom>(AICharacter->Controller);
    auto smartAIController = Cast<AAI_Smart_Controller_Custom>(AICharacter->Controller);

    if (smartAIController)
    {
        if (AICharacter->SelectedHotKeySlotNum == 2) {
            AICharacter->SM_GreenOnion->SetHiddenInGame(true);
            AICharacter->SM_Carrot->SetHiddenInGame(true);
            send_anim_packet(AICharacter->s_socket, Network::AnimType::DropSword);
        }

        if (AICharacter->mInventory->mSlots[1].Amount > 0)
            AICharacter->SelectedHotKeySlotNum = 1;
        else if(AICharacter->mInventory->mSlots[0].Amount > 0)
            AICharacter->SelectedHotKeySlotNum = 0;
    }

    ABaseCharacter* Target = nullptr;
    if(AIController)
        Target = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAIController_Custom::TargetKey));
    else if(smartAIController)
        Target = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AAI_Smart_Controller_Custom::TargetKey));

    if (nullptr == Target)
        return EBTNodeResult::Failed;
    if (Target->bIsDie)
        return EBTNodeResult::Failed;
    if (AICharacter->bIsDie)
        return EBTNodeResult::Failed;

    if(AICharacter->bStepBanana == false)
        AICharacter->Attack();
    IsAttacking = true;
    AICharacter->OnAttackEnd.AddLambda([this]()->void { IsAttacking = false; });

    //update KeyAmount(BTS_subtomato before)--------------------------------------------------------
    int FruitAmount = AICharacter->mInventory->mSlots[AICharacter->SelectedHotKeySlotNum].Amount;
    if (AIController)
        OwnerComp.GetBlackboardComponent()->SetValueAsInt(AAIController_Custom::AmountKey, FruitAmount);
    else if(smartAIController)
        OwnerComp.GetBlackboardComponent()->SetValueAsInt(AAI_Smart_Controller_Custom::AmountKey, FruitAmount);
    //----------------------------------------------------------------------------------------------

    return EBTNodeResult::InProgress;
    // ���� task�� ���� �ִϸ��̼��� ���� ������ ����ؾ� �ϴ� ���� task�̹Ƿ�
    // ExecuteTask�� ��� ���� InProgress�� ��ȯ�ϰ�, ������ ������ �� task�� �����ٰ� �˷��־�� ��.
    // �̸� �˷��ִ� �Լ��� FinishLatentTask. �� �Լ��� ȣ������ ������ �ൿƮ�� �ý����� ���� task�� ��� �ӹ��� �ִ´�.
    // �׷��� ȣ�� �� �� �ֵ��� ����� tick ����� Ȱ��ȭ�ϰ� ������ �ľ��� �� task ���� ������ �����־�� ��.
}

void UBTTaskNode_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    auto AICharacter = Cast<AAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == AICharacter)
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

    auto AIController = Cast<AAIController_Custom>(AICharacter->Controller);
    auto smartAIController = Cast<AAI_Smart_Controller_Custom>(AICharacter->Controller);

    if (AIController)
    {
        if (0 == AICharacter->mInventory->mSlots[AICharacter->SelectedHotKeySlotNum].Amount)
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        }
    }
    else if (smartAIController)
    {
        if (0 == AICharacter->mInventory->mSlots[AICharacter->SelectedHotKeySlotNum].Amount)
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
    }
    

    if (!IsAttacking)
    {
        //UE_LOG(LogTemp, Warning, TEXT("Finish!"));    

        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }

}
