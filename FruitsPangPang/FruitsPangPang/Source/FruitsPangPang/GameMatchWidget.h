// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameMatchWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UGameMatchWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
		class UButton* MatchButton;


	UFUNCTION()
		void TryMatching();
};
