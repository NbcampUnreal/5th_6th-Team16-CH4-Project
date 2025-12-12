// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UISubsystem.h"

#include "Blueprint/UserWidget.h"

UUISubsystem::UUISubsystem()
{
    static ConstructorHelpers::FObjectFinder<UUIConfig> ConfigObj(TEXT("/Game/Tarcopy/Main/Blueprints/UI/DA_UIConfig.DA_UIConfig"));
    if (ConfigObj.Succeeded())
    {
        UIConfigData = ConfigObj.Object;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UUISubsystem: Failed to find UIConfigData asset!"));
    }
}

UUserWidget* UUISubsystem::ShowUI(EUIType Type)
{
    // 이미 생성된 UI가 있을 경우
    if (TObjectPtr<UUserWidget>* Found = SingleWidgets.Find(Type))
    {
        (*Found)->SetVisibility(ESlateVisibility::Visible);
        return (*Found).Get();
    }
    
    // UI를 생성해야 하는 경우
    APlayerController* PC = GetLocalPlayer()->GetPlayerController(GetWorld());
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("UUISubsystem: PC is nullptr!"));
        return nullptr;
    }

    TSubclassOf<UUserWidget> WidgetClass = UIConfigData->GetWidgetClass(Type);
    UUserWidget* WidgetInstance = CreateWidget<UUserWidget>(PC, WidgetClass);
    if (!WidgetInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("UUISubsystem: WidgetInstance is nullptr!"));
        return nullptr;
    }

    WidgetInstance->AddToViewport();
    SingleWidgets.Add(Type, WidgetInstance);

    return WidgetInstance;
}

void UUISubsystem::HideUI(EUIType Type)
{
    if (TObjectPtr<UUserWidget>* Found = SingleWidgets.Find(Type))
    {
        (*Found)->SetVisibility(ESlateVisibility::Collapsed);
    }
}
