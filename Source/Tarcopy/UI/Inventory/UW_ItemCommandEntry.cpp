// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/UW_ItemCommandEntry.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Item/ItemCommand/ItemCommandBase.h"
#include "Kismet/KismetSystemLibrary.h"

void UUW_ItemCommandEntry::NativeDestruct()
{
	if (Btn)
	{
		Btn->OnClicked.RemoveAll(this);
	}
	Super::NativeDestruct();
}

void UUW_ItemCommandEntry::InitEntry(UItemCommandBase* InCommand)
{
	Command = InCommand;
	if (!IsValid(Command) || !Btn || !Txt)
	{
		return;
	}

	Txt->SetText(Command->TextDisplay);
	Btn->SetIsEnabled(Command->bExecutable);

	Btn->OnClicked.RemoveAll(this);
	Btn->OnClicked.AddDynamic(this, &ThisClass::HandleClicked);
}

void UUW_ItemCommandEntry::HandleClicked()
{
	if (!IsValid(Command))
	{
		return;
	}

	if (!Command->bExecutable)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Command is not executable"));
		return;
	}

	FItemCommandContext Ctx;
	Ctx.InstigatorController = GetOwningPlayer();
	if (Ctx.InstigatorController.IsValid())
	{
		Ctx.Instigator = Cast<AActor>(Ctx.InstigatorController->GetPawn());
	}

	Command->Execute(Ctx);
	OnExecuted.Broadcast();
}