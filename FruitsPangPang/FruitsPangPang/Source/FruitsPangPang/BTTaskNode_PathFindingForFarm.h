// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include <vector>
#include "BTTaskNode_PathFindingForFarm.generated.h"

class TreeInfo {
public:
	class ATree* mTree;
	bool bIgnored;	//ĳ�������� �������� ��� ignored�� ������ Ž���� ������ ���� ����
	TreeInfo();
	TreeInfo(class ATree* tree);
};
/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UBTTaskNode_PathFindingForFarm : public UBTTaskNode
{
	GENERATED_BODY()

public:
	std::vector<TreeInfo> trees;
	float fTreeDistance;
	int TargetTreeNum;
	UBTTaskNode_PathFindingForFarm();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;

	void FindTree();
};
