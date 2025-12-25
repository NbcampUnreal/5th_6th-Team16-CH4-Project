#include "Item/ItemCommand/DropCommand.h"
#include "Item/ItemInstance.h"
#include "Item/ItemWrapperActor/ItemWrapperActor.h"
#include "Item/Data/ItemData.h"
#include "Item/EquipComponent.h"

void UDropCommand::OnExecute(const FItemCommandContext& Context)
{
	if (OwnerItem.IsValid() == false)
		return;

	if (Context.Instigator.IsValid() == false)
		return;

	// 이거 InventoryComponent로 옮겨야 한다고 전달해주기 (테스트용으로 여기서 함)
	/*FVector SpawnLocation = Context.Instigator->GetActorLocation() + Context.Instigator->GetActorForwardVector() * 40.0f;
	AItemWrapperActor* ItemWrapperActor = GetWorld()->SpawnActor<AItemWrapperActor>(
		AItemWrapperActor::StaticClass(),
		SpawnLocation,
		FRotator::ZeroRotator);

	if (IsValid(ItemWrapperActor) == true)
	{
		ItemWrapperActor->SetItemInstance(OwnerItem.Get());
	}*/

	// 인벤토리랑 equip중에 어디서 떨구는 건지 파악하고 빼야 할 듯
	UEquipComponent* EquipComponent = Context.Instigator->FindComponentByClass<UEquipComponent>();
	if (IsValid(EquipComponent) == true)
	{
		EquipComponent->ServerRPC_UnequipItem(OwnerItem.Get());
		return;
	}
	// 여기까지
}
