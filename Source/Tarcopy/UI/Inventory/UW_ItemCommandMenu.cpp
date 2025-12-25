// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/UW_ItemCommandMenu.h"

#include "Components/VerticalBox.h"
#include "Item/ItemInstance.h"
#include "Item/ItemComponent/ItemComponentBase.h"
#include "Item/ItemCommand/ItemCommandBase.h"
#include "UI/Inventory/UW_ItemCommandEntry.h"

void UUW_ItemCommandMenu::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	SetKeyboardFocus();

	RebuildEntries();
}

void UUW_ItemCommandMenu::InitMenu(UItemInstance* InItem)
{
	Item = InItem;
	Commands.Reset();

	if (!Item.IsValid())
	{
		return;
	}

	const auto Components = Item->GetItemComponents();
	for (const auto& Comp : Components)
	{
		if (IsValid(Comp))
		{
			Comp->GetCommands(Commands);
		}
	}
}

void UUW_ItemCommandMenu::RebuildEntries()
{
	if (!PanelEntries)
	{
		return;
	}

	PanelEntries->ClearChildren();

	for (UItemCommandBase* Cmd : Commands)
	{
		if (!IsValid(Cmd) || !EntryClass)
		{
			continue;
		}

		UUW_ItemCommandEntry* Entry = CreateWidget<UUW_ItemCommandEntry>(GetOwningPlayer(), EntryClass);
		if (!IsValid(Entry))
		{
			continue;
		}

		PanelEntries->AddChildToVerticalBox(Entry);

		Entry->InitEntry(Cmd);
		Entry->OnExecuted.AddUObject(this, &ThisClass::HandleEntryExecuted);
	}
}

void UUW_ItemCommandMenu::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);
	CloseSelf();
}

void UUW_ItemCommandMenu::HandleEntryExecuted()
{
	CloseSelf();
}

void UUW_ItemCommandMenu::CloseSelf()
{
	RemoveFromParent();
}

