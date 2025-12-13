#include "Item/Interaction/InteractComponent.h"
#include "Item/Interaction/InteractionTask.h"

UInteractComponent::UInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// 평소엔 꺼둘 예정
	SetComponentTickEnabled(false);
}

void UInteractComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (const auto& Task : OnProgressTasks)
	{
		Task->OnProgressInteraction(DeltaTime);
	}

	for (const auto& Task : CompletedTasks)
	{
		OnProgressTasks.Remove(Task);
	}
	CompletedTasks.Empty();

	if (OnProgressTasks.IsEmpty() == true)
	{
		SetComponentTickEnabled(false);
	}
}

void UInteractComponent::OnTaskCompleted(UInteractionTask* Task)
{
	CompletedTasks.Add(Task);
}

void UInteractComponent::StartInteraction(AActor* Target, const FInteractionData& Data)
{
	if (OnProgressTasks.IsEmpty() == false && bCanMultitask == false)
		return;

	/*UInteractionTask* NewTask = NewObject<UInteractionTask>(this, Data.InteractionTaskClass);
	if (IsValid(NewTask) == false)
		return;

	SetComponentTickEnabled(true);
	NewTask->Init(GetOwner(), Data);
	NewTask->OnCompleted.BindUObject(this, &ThisClass::OnTaskCompleted);
	OnProgressTasks.Add(NewTask);*/
}

