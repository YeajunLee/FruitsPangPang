// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_Attack.h"
#include "AICharacter.h"
#include "AIController_Custom.h"

UBTTaskNode_Attack::UBTTaskNode_Attack()
{
    NodeName = TEXT("Attack_ThrowTomato");
    bNotifyTick = true; // ƽ ��� Ȱ��ȭ?
    IsAttacking = false;
}

EBTNodeResult::Type UBTTaskNode_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    auto AICharacter = Cast<AAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == AICharacter)
        return EBTNodeResult::Failed;

    AICharacter->Attack();
    IsAttacking = true;

    AICharacter->OnAttackEnd.AddLambda([this]()->void { IsAttacking = false; });

    return EBTNodeResult::InProgress;
    // ���� task�� ���� �ִϸ��̼��� ���� ������ ����ؾ� �ϴ� ���� task�̹Ƿ�
    // ExecuteTask�� ��� ���� InProgress�� ��ȯ�ϰ�, ������ ������ �� task�� �����ٰ� �˷��־�� ��.
    // �̸� �˷��ִ� �Լ��� FinishLatentTask. �� �Լ��� ȣ������ ������ �ൿƮ�� �ý����� ���� task�� ��� �ӹ��� �ִ´�.
    // �׷��� ȣ�� �� �� �ֵ��� ����� tick ����� Ȱ��ȭ�ϰ� ������ �ľ��� �� task ���� ������ �����־�� ��.
}

void UBTTaskNode_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
    if (!IsAttacking)
    {
        UE_LOG(LogTemp, Warning, TEXT("Finish!"));

        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
    
}