// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MessageBoxWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_Single_OneParam, int32, SomeParam);
/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UMessageBoxWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
		FDele_Single_OneParam MakeMessageBoxDelegate;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* mMsg;

	UFUNCTION(BlueprintCallable)
		void MakeMessageBox(const FString& msg);

	void MakeMessageBoxWithCode(const int& msgCode);
};
