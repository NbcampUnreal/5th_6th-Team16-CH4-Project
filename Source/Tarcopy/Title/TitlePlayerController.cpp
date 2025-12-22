// PlayerController for the title screen that spawns the title UI.

#include "Title/TitlePlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/UISubsystem.h"
#include "UI/UW_TitleScreen.h"

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

	TitleWidgetInstance = nullptr;
	TitleScreenWidget = nullptr;

	// Try UISubsystem (data-driven UI config).
	if (auto* LP = GetLocalPlayer())
	{
		if (auto* UIS = LP->GetSubsystem<UUISubsystem>())
		{
			UISubsystem = UIS;

			if (auto* Widget = UIS->ShowUI(EUIType::Title))
			{
				TitleWidgetInstance = Widget;
				TitleScreenWidget = Cast<UUW_TitleScreen>(Widget);

				if (IsValid(TitleScreenWidget))
				{
					TitleScreenWidget->OnJoinButtonClicked.AddDynamic(this, &ThisClass::HandleJoinRequested);
					TitleScreenWidget->OnOptionButtonClicked.AddDynamic(this, &ThisClass::HandleOptionRequested);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ATitlePlayerController: UISubsystem returned null widget for Title UI."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ATitlePlayerController: UISubsystem not available on LocalPlayer."));
		}
	}

	// Fallback to class property if subsystem path failed.
	if (!TitleWidgetInstance && IsValid(TitleWidgetClass) == true)
	{
		TitleWidgetInstance = CreateWidget<UUserWidget>(this, TitleWidgetClass); 
		if (IsValid(TitleWidgetInstance) == true)
		{
			TitleWidgetInstance->AddToViewport();

			TitleScreenWidget = Cast<UUW_TitleScreen>(TitleWidgetInstance.Get());
			if (IsValid(TitleScreenWidget))
			{
				TitleScreenWidget->OnJoinButtonClicked.AddDynamic(this, &ThisClass::HandleJoinRequested);
				TitleScreenWidget->OnOptionButtonClicked.AddDynamic(this, &ThisClass::HandleOptionRequested);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ATitlePlayerController: TitleWidgetClass is set but widget creation failed."));
		}
	}

	if (IsValid(TitleWidgetInstance))
	{
		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(TitleWidgetInstance->GetCachedWidget());
		SetInputMode(Mode);

		bShowMouseCursor = true;
	}
}

void ATitlePlayerController::JoinServer(const FString& InIPAddress)
{
	UE_LOG(LogTemp, Warning, TEXT("JoinServer Start"));
	FString Address = InIPAddress.TrimStartAndEnd();

	if (Address.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("JoinServer skipped: empty IP/Port input."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Attempting to join server via OpenLevel: %s"), *Address);
	UGameplayStatics::OpenLevel(GetWorld(), FName(*Address), true);
}

void ATitlePlayerController::HandleJoinRequested(const FText& InIpPort)
{
	JoinServer(InIpPort.ToString());
}

void ATitlePlayerController::HandleOptionRequested()
{
	if (!OptionsWidgetInstance && IsValid(OptionsWidgetClass))
	{
		OptionsWidgetInstance = CreateWidget<UUserWidget>(this, OptionsWidgetClass);
		if (IsValid(OptionsWidgetInstance))
		{
			OptionsWidgetInstance->AddToViewport();
		}
	}

	if (!IsValid(OptionsWidgetInstance))
	{
		UE_LOG(LogTemp, Warning, TEXT("ATitlePlayerController: OptionsWidgetClass is not set or failed to create."));
		return;
	}

	const bool bShow = !OptionsWidgetInstance->IsVisible();
	OptionsWidgetInstance->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	FInputModeUIOnly Mode;
	if (bShow)
	{
		Mode.SetWidgetToFocus(OptionsWidgetInstance->GetCachedWidget());
	}
	else if (IsValid(TitleWidgetInstance))
	{
		Mode.SetWidgetToFocus(TitleWidgetInstance->GetCachedWidget());
	}

	SetInputMode(Mode);
	bShowMouseCursor = true;
}
