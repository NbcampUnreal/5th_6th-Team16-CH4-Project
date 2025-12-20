#include "Item/ItemCommand/DropCommand.h"
#include "Item/ItemInstance.h"
#include "Item/ItemWrapperActor/ItemWrapperActor.h"
#include "Item/Data/ItemData.h"

void UDropCommand::OnExecute(const FItemCommandContext& Context)
{
	if (OwnerItem.IsValid() == false)
		return;

	if (Context.Instigator.IsValid() == false)
		return;

	// 이거 InventoryComponent로 옮겨야 한다고 전달해주기 (테스트용으로 여기서 함)
	FVector SpawnLocation = Context.Instigator->GetActorLocation() + Context.Instigator->GetActorForwardVector() * 40.0f;
	AItemWrapperActor* ItemWrapperActor = GetWorld()->SpawnActor<AItemWrapperActor>(
		AItemWrapperActor::StaticClass(),
		SpawnLocation,
		FRotator::ZeroRotator);

	if (IsValid(ItemWrapperActor) == true)
	{
		ItemWrapperActor->SetItemInstance(OwnerItem.Get());
	}
	// 여기까지
}
