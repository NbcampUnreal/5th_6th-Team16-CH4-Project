// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UISubsystem.h"

#include "Blueprint/UserWidget.h"
#include "UI/UW_RootHUD.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

UUISubsystem::UUISubsystem()
{
    static ConstructorHelpers::FObjectFinder<UUIConfig> ConfigObj(TEXT("/Game/Tarcopy/Main/Blueprints/UI/DA_UIConfig.DA_UIConfig"));
    if (ConfigObj.Succeeded())
    {
        UIConfigData = ConfigObj.Object;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UUISubsystem(): Failed to find UIConfigData asset!"));
    }
}

void UUISubsystem::PlayerControllerChanged(APlayerController* NewPlayerController)
{
    Super::PlayerControllerChanged(NewPlayerController);

    InitRootHUD();
}

UUserWidget* UUISubsystem::ShowUI(EUIType Type)
{
    // 이미 생성된 UI가 있을 경우
    if (TObjectPtr<UUserWidget>* Found = SingleWidgets.Find(Type))
    {
        (*Found)->SetVisibility(ESlateVisibility::Visible);
        return Found->Get();
    }
    
    // UI를 생성해야 하는 경우
    APlayerController* PC = GetLocalPlayer()->GetPlayerController(GetWorld());
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("UUISubsystem::ShowUI: PC is nullptr!"));
        return nullptr;
    }

    FUIInfo WidgetInfo;
    if (!UIConfigData->GetInfo(Type, WidgetInfo))
    {
        UE_LOG(LogTemp, Error, TEXT("UUISubsystem::ShowUI: Cannot Find UIConfig Info!"));
        return nullptr;
    }
    
    UUserWidget* WidgetInstance = CreateWidget<UUserWidget>(PC, WidgetInfo.WidgetClass);
    if (!WidgetInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("UUISubsystem::ShowUI: WidgetInstance is nullptr!"));
        return nullptr;
    }

    if (!RootHUD)
    {
        InitRootHUD();
    }
    UCanvasPanelSlot* Slot = RootHUD->GetRootCanvas()->AddChildToCanvas(WidgetInstance);
    if (!Slot)
    {
        UE_LOG(LogTemp, Error, TEXT("UUISubsystem::ShowUI: Slot is nullptr!"));
        return nullptr;
    }
    ApplyLayoutPreset(Slot, WidgetInfo.Layout);

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

void UUISubsystem::InitRootHUD()
{
    if (RootHUD)
    {
        return;
    }

    APlayerController* PC = GetLocalPlayer()->GetPlayerController(GetWorld());
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("UUISubsystem::InitRootHUD: PC is null!"));
        return;
    }

    FUIInfo RootInfo;
    if (!UIConfigData->GetInfo(EUIType::Root, RootInfo))
    {
        UE_LOG(LogTemp, Error, TEXT("UUISubsystem::InitRootHUD: Cannot Find UIConfig Info!"));
        return;
    }

    RootHUD = CreateWidget<UUW_RootHUD>(PC, RootInfo.WidgetClass);
    if (RootHUD == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UUISubsystem::InitRootHUD: RootHUD is null!"));
        return;
    }

    RootHUD->AddToViewport();
}

void UUISubsystem::ApplyLayoutPreset(UCanvasPanelSlot* Slot, const FUILayoutPreset& Layout)
{
    Slot->SetAutoSize(Layout.bAutoSize);
    Slot->SetAnchors(Layout.Anchors);
    Slot->SetAlignment(Layout.Alignment);
    Slot->SetPosition(Layout.Position);

    if (!Layout.bAutoSize)
    {
        Slot->SetSize(Layout.Size);
    }

    Slot->SetZOrder(Layout.ZOrder);
}
