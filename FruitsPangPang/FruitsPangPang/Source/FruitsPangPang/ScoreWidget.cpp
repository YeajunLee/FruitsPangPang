// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreWidget.h"
#include <algorithm>
#include "Network.h"
#include "MyCharacter.h"
#include "Components/TextBlock.h"

using namespace std;

ScoreInfo::ScoreInfo()
	:mCharacter(nullptr)
	,score(0)
{

}

ScoreInfo::ScoreInfo(class ABaseCharacter* character)
	:mCharacter(character)
	,score(mCharacter->score)
{

}

void ScoreInfo::UpdateScore()
{
	score = (mCharacter->killcount * 2) - mCharacter->deathcount;
}

void UScoreWidget::NativePreConstruct()
{
	RankText.Add(FText::FromString("1st"));
	RankText.Add(FText::FromString("2nd"));
	RankText.Add(FText::FromString("3rd"));
	RankText.Add(FText::FromString("4th"));
	RankText.Add(FText::FromString("5th"));
	RankText.Add(FText::FromString("6th"));
	RankText.Add(FText::FromString("7th"));
	RankText.Add(FText::FromString("8th"));
#define LOCTEXT_NAMESPACE "score"
	No_1->SetText(FText::FromString("1st"));
	No_2->SetText(FText::FromString("2nd"));
	No_3->SetText(FText::FromString("3rd"));
	No_4->SetText(FText::FromString("4th"));
	No_5->SetText(FText::FromString("5th"));
	No_6->SetText(FText::FromString("6th"));
	No_7->SetText(FText::FromString("7th"));
	No_8->SetText(FText::FromString("8th"));

	if(nullptr!= Network::GetNetwork()->mMyCharacter)
		ScoreBoard.push_back(ScoreInfo(Network::GetNetwork()->mMyCharacter));
	UpdateRank();



#undef LOCTEXT_NAMESPACE

}

void UScoreWidget::NativeDestruct()
{
	ScoreBoard.clear();
}

void UScoreWidget::UpdateScoreBoard()
{
	for (auto& scoreInfo : ScoreBoard)
	{
		scoreInfo.UpdateScore();
	}
}

void UScoreWidget::UpdateRank()
{
	UpdateScoreBoard();
	sort(ScoreBoard.begin(), ScoreBoard.end());

#define LOCTEXT_NAMESPACE "score"
	int judgeSameRank = TNumericLimits<int>::Max();
	int skiprank = 0;
	for (int i = 0,samerank = 1; i < ScoreBoard.size(); ++i)
	{
		if (Network::GetNetwork()->mMyCharacter->c_id == ScoreBoard[i].GetCharacter()->c_id)
			MyRank->SetText(FText::Format(LOCTEXT("score", "{0}/{1}"), i + 1, ScoreBoard.size()));
		if (judgeSameRank == ScoreBoard[i].GetScore())
		{
			skiprank++;
		}
		else
		{
			samerank += skiprank;
			skiprank = 1;
		}
		judgeSameRank = ScoreBoard[i].GetScore();
		switch (i)
		{
		case 0:
			No_1->SetText(RankText[samerank - 1]);
			No_1Name->SetText(FText::Format(LOCTEXT("score", "{0}"), FText::FromString(ScoreBoard[i].GetCharacter()->CharacterName)));
			No_1Score->SetText(FText::Format(LOCTEXT("score", "({0})"), ScoreBoard[i].GetScore()));
			break;
		case 1:
			No_2->SetText(RankText[samerank - 1]);
			No_2Name->SetText(FText::Format(LOCTEXT("score", "{0}"), FText::FromString(ScoreBoard[i].GetCharacter()->CharacterName)));
			No_2Score->SetText(FText::Format(LOCTEXT("score", "({0})"), ScoreBoard[i].GetScore()));
			break;
		case 2:
			No_3->SetText(RankText[samerank - 1]);
			No_3Name->SetText(FText::Format(LOCTEXT("score", "{0}"), FText::FromString(ScoreBoard[i].GetCharacter()->CharacterName)));
			No_3Score->SetText(FText::Format(LOCTEXT("score", "({0})"), ScoreBoard[i].GetScore()));
			break;
		case 3:
			No_4->SetText(RankText[samerank - 1]);
			No_4Name->SetText(FText::Format(LOCTEXT("score", "{0}"), FText::FromString(ScoreBoard[i].GetCharacter()->CharacterName)));
			No_4Score->SetText(FText::Format(LOCTEXT("score", "({0})"), ScoreBoard[i].GetScore()));
			break;
		case 4:
			No_5->SetText(RankText[samerank - 1]);
			No_5Name->SetText(FText::Format(LOCTEXT("score", "{0}"), FText::FromString(ScoreBoard[i].GetCharacter()->CharacterName)));
			No_5Score->SetText(FText::Format(LOCTEXT("score", "({0})"), ScoreBoard[i].GetScore()));
			break;
		case 5:
			No_6->SetText(RankText[samerank - 1]);
			No_6Name->SetText(FText::Format(LOCTEXT("score", "{0}"), FText::FromString(ScoreBoard[i].GetCharacter()->CharacterName)));
			No_6Score->SetText(FText::Format(LOCTEXT("score", "({0})"), ScoreBoard[i].GetScore()));
			break;
		case 6:
			No_7->SetText(RankText[samerank - 1]);
			No_7Name->SetText(FText::Format(LOCTEXT("score", "{0}"), FText::FromString(ScoreBoard[i].GetCharacter()->CharacterName)));
			No_7Score->SetText(FText::Format(LOCTEXT("score", "({0})"), ScoreBoard[i].GetScore()));
			break;
		case 7:
			No_8->SetText(RankText[samerank - 1]);
			No_8Name->SetText(FText::Format(LOCTEXT("score", "{0}"), FText::FromString(ScoreBoard[i].GetCharacter()->CharacterName)));
			No_8Score->SetText(FText::Format(LOCTEXT("score", "({0})"), ScoreBoard[i].GetScore()));
			break;

		}
	}
	for (int i = ScoreBoard.size(); i < MAX_USER; ++i)
	{
		switch (i)
		{
		case 0:
			No_1->SetText(FText::FromString("-"));
			No_1Name->SetText(FText::FromString("NoName"));
			No_1Score->SetText(FText::FromString("(-)"));
			break;
		case 1:
			No_2->SetText(FText::FromString("-"));
			No_2Name->SetText(FText::FromString("NoName"));
			No_2Score->SetText(FText::FromString("(-)"));
			break;
		case 2:
			No_3->SetText(FText::FromString("-"));
			No_3Name->SetText(FText::FromString("NoName"));
			No_3Score->SetText(FText::FromString("(-)"));
			break;
		case 3:
			No_4->SetText(FText::FromString("-"));
			No_4Name->SetText(FText::FromString("NoName"));
			No_4Score->SetText(FText::FromString("(-)"));
			break;
		case 4:
			No_5->SetText(FText::FromString("-"));
			No_5Name->SetText(FText::FromString("NoName"));
			No_5Score->SetText(FText::FromString("(-)"));
			break;
		case 5:
			No_6->SetText(FText::FromString("-"));
			No_6Name->SetText(FText::FromString("NoName"));
			No_6Score->SetText(FText::FromString("(-)"));
			break;
		case 6:
			No_7->SetText(FText::FromString("-"));
			No_7Name->SetText(FText::FromString("NoName"));
			No_7Score->SetText(FText::FromString("(-)"));
			break;
		case 7:
			No_8->SetText(FText::FromString("-"));
			No_8Name->SetText(FText::FromString("NoName"));
			No_8Score->SetText(FText::FromString("(-)"));
			break;

		}
	}
#undef LOCTEXT_NAMESPACE
}