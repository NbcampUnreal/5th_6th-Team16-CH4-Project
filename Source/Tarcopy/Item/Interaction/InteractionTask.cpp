#include "Item/Interaction/InteractionTask.h"
#include "Item/EquipComponent.h"

//void UInteractionTask::Init(AActor* InInstigator, const FInteractionData& InData)
//{
//	Instigator = InInstigator;
//	Data = InData;
//}

void UInteractionTask::OnProgressInteraction(float DeltaTime)
{
	ElapsedTime += DeltaTime;
	if (ElapsedTime >= Duration)
	{
		OnInteractionCompleted();
	}
}

void UInteractionTask::OnInteractionCompleted()
{
	if (OnCompleted.IsBound() == true)
	{
		OnCompleted.Execute(this);
	}

	ReduceDurability();
}

void UInteractionTask::ReduceDurability()
{
	UEquipComponent* Equip = Instigator->FindComponentByClass<UEquipComponent>();
	if (IsValid(Equip) == false)
		return;

	// 장착된 아이템 내구도 감소
	//Equip->
}
