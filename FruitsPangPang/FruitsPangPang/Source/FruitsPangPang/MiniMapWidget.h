// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniMapWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UMiniMapWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UOverlay* MapOverlay;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* MapIMG;
};
