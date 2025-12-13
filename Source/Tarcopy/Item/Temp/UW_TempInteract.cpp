#include "Item/Temp/UW_TempInteract.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Item/ItemComponent/ItemComponentInteractionData.h"

void UUW_TempInteract::SetInteract(const FItemComponentInteractionData& InData)
{
	Data = InData;

	TextInteract->SetText(InData.TextDisplay);
	BtnInteract->OnClicked.AddDynamic(this, &ThisClass::ExecuteInteract);
	// bIsInteractable에 따라서 버튼 활성화/비활성화 (색 바뀜 포함)
}

void UUW_TempInteract::ExecuteInteract()
{
	if (Data.DelegateInteract.IsBound() == true)
	{
		Data.DelegateInteract.Execute();
	}
}
