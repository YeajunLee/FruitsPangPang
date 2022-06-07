// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerIconWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UPlayerIconWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/*virtual void NativePreConstruct() override;

	virtual void NativeTick(const FGeometry& Geometry, float DeltaSeconds) override;*/


	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UOverlay* Overlay0;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* PlayerIcon;
};
