// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RespawnWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API URespawnWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button0_0;
	UPROPERTY(meta = (BindWidget))
		class UButton* Button1_0;
	UPROPERTY(meta = (BindWidget))
		class UButton* Button2_0;
	UPROPERTY(meta = (BindWidget))
		class UButton* Button0_1;
	UPROPERTY(meta = (BindWidget))
		class UButton* Button1_1;
	UPROPERTY(meta = (BindWidget))
		class UButton* Button2_1;
	UPROPERTY(meta = (BindWidget))
		class UButton* Button0_2;
	UPROPERTY(meta = (BindWidget))
		class UButton* Button1_2;
	UPROPERTY(meta = (BindWidget))
		class UButton* Button2_2;

	UFUNCTION()
		void BClick0_0();
	UFUNCTION()
		void BClick1_0();
	UFUNCTION()
		void BClick2_0();
	UFUNCTION()
		void BClick0_1();
	UFUNCTION()
		void BClick1_1();
	UFUNCTION()
		void BClick2_1();
	UFUNCTION()
		void BClick0_2();
	UFUNCTION()
		void BClick1_2();
	UFUNCTION()
		void BClick2_2();
};
