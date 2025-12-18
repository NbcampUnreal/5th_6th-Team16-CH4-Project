#include "Item/ItemCommand/ItemCommandBase.h"
#include "Kismet/KismetSystemLibrary.h"

void UItemCommandBase::Execute(const FItemCommandContext& Context)
{
	if (bExecutable == false)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("This command is not executable"));
		return;
	}

	OnExecute(Context);
}
