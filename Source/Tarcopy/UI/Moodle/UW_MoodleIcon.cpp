// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Moodle/UW_MoodleIcon.h"

#include "Components/ProgressBar.h"

void UUW_MoodleIcon::SetRatio(float InRatio)
{
	BackgroundFill->SetPercent(InRatio);
	BackgroundFill->SetFillColorAndOpacity(GetMoodColor(InRatio));
}

FLinearColor UUW_MoodleIcon::GetMoodColor(float Ratio)
{
	if (Ratio > 0.66f) 
	{
		return FLinearColor::Green;
	}
	if (Ratio > 0.33f)
	{
		return FLinearColor::Yellow;
	}
	return FLinearColor::Red;
}