// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "BaseLVBP.generated.h"

/**
 * 
 */
UCLASS()
class FPS_CPP_API ABaseLVBP : public ALevelScriptActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void Conn(UPARAM(ref) ACharacter* charRef);
	UFUNCTION(BlueprintCallable)
	void DisConn();
};
