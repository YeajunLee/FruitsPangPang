// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreWidget.generated.h"

/**
 * 
 */

class ScoreInfo {
private:
	class ABaseCharacter* mCharacter;
	int score;
public:
	ScoreInfo();
	ScoreInfo(class ABaseCharacter* character);
	void UpdateScore();
	bool operator<(ScoreInfo& other) const {
		return this->score < other.score;
	}
	const class ABaseCharacter* GetCharacter() const { return mCharacter; }
	const int GetScore() const { return score; }
};

UCLASS()
class FRUITSPANGPANG_API UScoreWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;



	std::vector<ScoreInfo>  ScoreBoard;
	void UpdateRank();
	void UpdateScoreBoard();

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
