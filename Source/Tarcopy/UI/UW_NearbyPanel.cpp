// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UW_NearbyPanel.h"

#include "Components/ScrollBox.h"
#include "Inventory/LootScannerComponent.h"
#include "Inventory/ContainerActor.h"
#include "UI/UISubsystem.h"
#include "UI/UW_InventoryBorder.h"
#include "UI/UW_Inventory.h"
#include "Components/NamedSlot.h"
#include "Components/Button.h"
#include "Blueprint/WidgetTree.h"
#include "Inventory/UW_ContainerBtn.h"

void UUW_NearbyPanel::NativeConstruct()
{
	Super::NativeConstruct();

	if (!SelectedContainer)
	{
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}
	
	if (auto* LP = PC->GetLocalPlayer())
	{
		if (auto* UIS = LP->GetSubsystem<UUISubsystem>())
		{
			if (auto* Border = UIS->ShowUI(EUIType::InventoryBorder))
			{
				if (auto* Inventory = UIS->ShowUI(EUIType::Inventory))
				{
					SelectedContainer->SetContent(Border);
					InventoryBorder = Cast<UUW_InventoryBorder>(Border);
					InventoryBorder->SetContentWidget(Inventory);
					InventoryWidget = Cast<UUW_Inventory>(Inventory);
					SelectedContainer->SetVisibility(ESlateVisibility::Collapsed);
				}
			}
		}
	}
}

void UUW_NearbyPanel::BindScanner(ULootScannerComponent* InScanner)
{
	if (!InScanner)
	{
		return;
	}

	BoundScanner = InScanner;
	BoundScanner->OnScannedContainersChanged.AddUObject(this, &UUW_NearbyPanel::RefreshContainerList);
	BoundScanner->OnScannedGroundChanged.AddUObject(this, &UUW_NearbyPanel::HandleGroundUpdatedWhileOpen);

	RefreshContainerList();
}

void UUW_NearbyPanel::RefreshContainerList()
{
	if (!BoundScanner || !ContainerScrollBox)
	{
		return;
	}

	if (bInventoryPanelOpen && !bLastSelectedWasGround)
	{
		AContainerActor* Selected = LastSelectedContainer.Get();
		bool bStillOverlapped = false;

		if (IsValid(Selected))
		{
			for (const TWeakObjectPtr<AContainerActor>& C : BoundScanner->OverlappedContainerActors)
			{
				if (C.Get() == Selected)
				{
					bStillOverlapped = true;
					break;
				}
			}
		}

		if (!bStillOverlapped)
		{
			SelectedContainer->SetVisibility(ESlateVisibility::Collapsed);
			bInventoryPanelOpen = false;
			LastSelectedContainer = nullptr;
		}
	}

	ContainerScrollBox->ClearChildren();

	for (const TWeakObjectPtr<AContainerActor>& Container : BoundScanner->OverlappedContainerActors)
	{
		if (!IsValid(Container.Get()))
		{
			continue;
		}

		UUW_ContainerBtn* Button = CreateWidget<UUW_ContainerBtn>(GetOwningPlayer(), ContainerBtnClass);
		Button->BindContainer(Container.Get());
		Button->OnClickedWithContainer.AddUObject(this, &UUW_NearbyPanel::HandleContainerSelected);

		ContainerScrollBox->AddChild(Button);
	}

	UButton* GroundBtn = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	GroundBtn->OnClicked.AddDynamic(this, &UUW_NearbyPanel::HandleGroundSelected);

	ContainerScrollBox->AddChild(GroundBtn);
}

void UUW_NearbyPanel::HandleContainerSelected(AContainerActor* Container)
{
	if (!IsValid(Container) || !InventoryWidget)
	{
		return;
	}

	const bool bSameAsLast = bInventoryPanelOpen &&	!bLastSelectedWasGround && LastSelectedContainer.Get() == Container;
	if (bSameAsLast)
	{
		SelectedContainer->SetVisibility(ESlateVisibility::Collapsed);
		bInventoryPanelOpen = false;
		return;
	}

	SelectedContainer->SetVisibility(ESlateVisibility::Visible);
	InventoryWidget->BindInventory(Container->GetInventoryData());

	bInventoryPanelOpen = true;
	bLastSelectedWasGround = false;
	LastSelectedContainer = Container;
}

void UUW_NearbyPanel::HandleGroundSelected()
{
	if (!BoundScanner || !InventoryWidget)
	{
		return;
	}

	const bool bSameAsLast = bInventoryPanelOpen && bLastSelectedWasGround;
	if (bSameAsLast)
	{
		SelectedContainer->SetVisibility(ESlateVisibility::Collapsed);
		bInventoryPanelOpen = false;
		return;
	}

	SelectedContainer->SetVisibility(ESlateVisibility::Visible);
	InventoryWidget->BindInventory(BoundScanner->GetGroundInventoryData());

	bInventoryPanelOpen = true;
	bLastSelectedWasGround = true;
	LastSelectedContainer = nullptr;
}

void UUW_NearbyPanel::HandleGroundUpdatedWhileOpen()
{
	if (!bInventoryPanelOpen || !bLastSelectedWasGround)
	{
		return;
	}

	if (!BoundScanner || !InventoryWidget)
	{
		return;
	}

	InventoryWidget->RefreshItems();
}
