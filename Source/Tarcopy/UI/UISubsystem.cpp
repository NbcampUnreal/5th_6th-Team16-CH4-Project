// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UISubsystem.h"

#include "Blueprint/UserWidget.h"
#include "UI/UW_RootHUD.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Inventory/InventoryData.h"
#include "Misc/Guid.h"
#include "UI/UW_Inventory.h"
#include "UI/UW_InventoryBorder.h"

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

UUW_Inventory* UUISubsystem::ShowInventoryUI(UInventoryData* InventoryData)
{
    // UI를 생성해야 하는 경우
    APlayerController* PC = GetLocalPlayer()->GetPlayerController(GetWorld());
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("UUISubsystem::ShowUI: PC is nullptr!"));
        return nullptr;
    }

    FUIInfo WidgetInfo;
    if (!UIConfigData->GetInfo(EUIType::Inventory, WidgetInfo))
    {
        UE_LOG(LogTemp, Error, TEXT("UUISubsystem::ShowUI: Cannot Find UIConfig Info!"));
        return nullptr;
    }

    UUW_Inventory* InventoryInstance = CreateWidget<UUW_Inventory>(PC, WidgetInfo.WidgetClass);
    if (!InventoryInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("UUISubsystem::ShowUI: InventoryInstance is nullptr!"));
        return nullptr;
    }

    InventoryInstance->BindInventory(InventoryData);

    FUIInfo BorderWidgetInfo;
    if (!UIConfigData->GetInfo(EUIType::InventoryBorder, BorderWidgetInfo))
    {
        UE_LOG(LogTemp, Error, TEXT("UUISubsystem::ShowUI: Cannot Find UIConfig Info!"));
        return nullptr;
    }

    UUW_InventoryBorder* BorderInstance = CreateWidget<UUW_InventoryBorder>(PC, BorderWidgetInfo.WidgetClass);
    if (!BorderInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("UUISubsystem::ShowUI: BorderInstance is nullptr!"));
        return nullptr;
    }

    BorderInstance->SetContentWidget(InventoryInstance);

    if (!RootHUD)
    {
        InitRootHUD();
    }
    UCanvasPanelSlot* Slot = RootHUD->GetRootCanvas()->AddChildToCanvas(BorderInstance);
    if (!Slot)
    {
        UE_LOG(LogTemp, Error, TEXT("UUISubsystem::ShowUI: Slot is nullptr!"));
        return nullptr;
    }
    ApplyLayoutPreset(Slot, WidgetInfo.Layout);
    Slot->SetPosition(FVector2D(WidgetInfo.Layout.Position.X + InventoryWidgets.Num() * 10, WidgetInfo.Layout.Position.Y + InventoryWidgets.Num() * 10));

    FGuid InventoryID = InventoryData->GetID();
    InventoryWidgets.Add(InventoryID, InventoryInstance);

    return InventoryInstance;
}

void UUISubsystem::HideInventoryUI(FGuid InventoryID)
{
    InventoryWidgets[InventoryID]->RemoveFromParent();
    InventoryWidgets.Remove(InventoryID);
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
