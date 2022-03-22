// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "InGameAiLv.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API AInGameAiLv : public ALevelScriptActor
{
	GENERATED_BODY()
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	//UFUNCTION(BlueprintCallable)
	void ConnAi();
	
};
