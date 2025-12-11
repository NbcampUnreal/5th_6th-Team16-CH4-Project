// Title screen widget: start, options, quit, and server IP join.

#include "UI/UW_TitleLayout.h"

#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Title/TitlePlayerController.h"

UUW_TitleLayout::UUW_TitleLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UUW_TitleLayout::NativeConstruct()
{
	PlayButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnPlayButtonClicked);
	ExitButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
}

void UUW_TitleLayout::OnPlayButtonClicked()
{
	ATitlePlayerController* PlayerController = GetOwningPlayer<ATitlePlayerController>();
	
	if (IsValid(PlayerController) == true)
	{
		FText ServerIP = ServerIPEditableText->GetText();
		PlayerController->JoinServer(ServerIP.ToString());
	}
}

void UUW_TitleLayout::OnOptionButtonClicked()
{
}

void UUW_TitleLayout::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
