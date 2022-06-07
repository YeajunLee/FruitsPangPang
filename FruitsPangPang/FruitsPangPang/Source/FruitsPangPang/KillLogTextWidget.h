// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KillLogTextWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UKillLogTextWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& my, float deltatime) override;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* mAttacker;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* mVictim;

	void MakeKillLog(const FString& attacker, const FString& victim);
	float LifeTime = 5.0f;
};
