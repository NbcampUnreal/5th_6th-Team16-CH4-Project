// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UIConfig.h"

#include "Blueprint/UserWidget.h"

TSubclassOf<UUserWidget> UUIConfig::GetWidgetClass(EUIType Type) const
{
    if (const TSubclassOf<UUserWidget> Found = WidgetClasses.FindChecked(Type))
    {
        return Found;
    }
    return nullptr;
}
