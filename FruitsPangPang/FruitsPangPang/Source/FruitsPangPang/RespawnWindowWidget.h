// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RespawnWindowWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API URespawnWindowWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& my, float deltatime) override;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* RemainTimeText;


	UPROPERTY(meta = (BindWidget))
		class UVerticalBox* MinimapBox;

	class URespawnWidget* mRespawnWidget;
	const float RespawnTime = 5.0f;
	float RemainTime;
	bool bActivate;
};
