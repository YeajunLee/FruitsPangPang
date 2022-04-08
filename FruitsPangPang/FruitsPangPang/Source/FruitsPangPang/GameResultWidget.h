// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class FRUITSPANGPANG_API UGameResultWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	class UScoreWidget* mScoreWidget;


	std::vector<class UTextBlock*>  mTextBlockRank;
	std::vector<class UTextBlock*>  mTextBlockName;
	std::vector<class UTextBlock*>  mTextBlockScore;

	UPROPERTY(meta = (BindWidget))
		class UButton* LobbyButton;


	UFUNCTION()
		void GoToLobby();


	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_1;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_1Name;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_1Score;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_2;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_2Name;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_2Score;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_3;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_3Name;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_3Score;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_4;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_4Name;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_4Score;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_5;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_5Name;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_5Score;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_6;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_6Name;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_6Score;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_7;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_7Name;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_7Score;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_8;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_8Name;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* No_8Score;
};
