// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatTextWidget.h"
#include "Components/TextBlock.h"
#include "Fonts/FontMeasure.h"

void UChatTextWidget::NativePreConstruct()
{
}

void UChatTextWidget::NativeDestruct()
{

}



void UChatTextWidget::MakeChatText(const FString& name, const FString& msg)
{

#define LOCTEXT_NAMESPACE "chattext"
	mText->SetText(FText::Format(LOCTEXT("chattext", "{0}:{1}"), FText::FromString(name), FText::FromString(msg)));
#undef LOCTEXT_NAMESPACE

	mText->SetText(FText::FromString(WrapTextTooLongWord(mText->Text, 650)));

}


FString UChatTextWidget::WrapTextTooLongWord(FText& text, float wrapsize)
{

	const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

	FString str = text.ToString();
	int last_newline = -1;
	FString testLine;
	//������ ������ġ�������� ���ڸ� �ϳ��� �����鼭 fontũ�⿡ ���� wrapsize�� ���Ѵ�.
	//����� �Ѿ�� �ش� ��ġ�� ���๮�ڸ� �־��ְ�, ������ ������ġ�� ����
	for (int i = 0; i < str.Len(); i++) {
		testLine = str.Mid(last_newline + 1, i - last_newline);
		if (FontMeasure->Measure(testLine, mText->Font).X > wrapsize) {
			str.InsertAt(i, '\n');
			last_newline = i;
		}

	}
	return str;
}