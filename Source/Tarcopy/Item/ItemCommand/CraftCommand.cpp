#include "Item/ItemCommand/CraftCommand.h"
#include "Item/CraftSubsystem.h"
#include "Item/DataTableSubsystem.h"

void UCraftCommand::OnExecute(const FItemCommandContext& Context)
{
	// 인벤토리에서 재료 아이템 제거, 결과 아이템 추가
	UDataTableSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	if (IsValid(DataTableSubsystem) == false)
		return;

}
