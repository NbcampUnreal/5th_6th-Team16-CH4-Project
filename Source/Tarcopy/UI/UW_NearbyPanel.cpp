// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UW_NearbyPanel.h"

#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Inventory/LootScannerComponent.h"
#include "Inventory/ContainerActor.h"

void UUW_NearbyPanel::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUW_NearbyPanel::BindScanner(ULootScannerComponent* InScanner)
{
	if (!InScanner)
	{
		return;
	}

	BoundScanner = InScanner;
	BoundScanner->OnScannedContainersChanged.AddUObject(this, &UUW_NearbyPanel::RefreshContainerList);

	RefreshContainerList();
}

void UUW_NearbyPanel::RefreshContainerList()
{
	if (!BoundScanner || !ContainerScrollBox)
	{
		return;
	}

	ContainerScrollBox->ClearChildren();

	for (const TWeakObjectPtr<AContainerActor>& Container : BoundScanner->OverlappedContainerActors)
	{
		if (!IsValid(Container.Get()))
		{
			continue;
		}

		UButton* Button = NewObject<UButton>(ContainerScrollBox);
		UTextBlock* Text = NewObject<UTextBlock>(Button);

		Text->SetText(Container->GetDisplayName());
		Button->AddChild(Text);

		ContainerScrollBox->AddChild(Button);
	}
}
