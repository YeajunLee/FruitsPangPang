// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "InGameLv.generated.h"

/**
 * 
 */
UCLASS()
class AICLIENT_API AInGameLv : public ALevelScriptActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void ConnAi();
};
