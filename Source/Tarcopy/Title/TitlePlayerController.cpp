// PlayerController for the title screen that spawns the title UI.

#include "Title/TitlePlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

ATitlePlayerController::ATitlePlayerController()
	: TitleWidgetClass(nullptr)
	, TitleWidget(nullptr)
{
	bShowMouseCursor = true;
}

void ATitlePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalController() == false)
	{
		return;
	}

	if (IsValid(TitleWidgetClass) == true)
	{
		TitleWidgetInstance = CreateWidget<UUserWidget>(this, TitleWidgetClass); 
		if (IsValid(TitleWidgetInstance) == true)
		{
			TitleWidgetInstance->AddToViewport();

			FInputModeUIOnly Mode;
			Mode.SetWidgetToFocus(TitleWidgetInstance->GetCachedWidget());
			SetInputMode(Mode);

			bShowMouseCursor = true;
		}
	}
}

void ATitlePlayerController::JoinServer(const FString& InIPAddress)
{
	FName NextLevelName = FName(*InIPAddress);
	UGameplayStatics::OpenLevel(GetWorld(), NextLevelName, true);
}
