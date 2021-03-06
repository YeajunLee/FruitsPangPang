// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* id;

	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* Password;

	UPROPERTY(meta = (BindWidget))
		class UButton* LoginButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* SignUpButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAudioComponent* mLoginBGM;
	UFUNCTION()
		void TryLogin();
	UFUNCTION()
		void TrySignUp();
	void SoundStop();
public:
};
