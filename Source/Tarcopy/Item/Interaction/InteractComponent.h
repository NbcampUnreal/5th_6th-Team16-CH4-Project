#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractComponent.generated.h"

class UInteractionTask;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TARCOPY_API UInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OnTaskCompleted(UInteractionTask* Task);
	// void CancelTask();

public:
	void StartInteraction(AActor* Target, const struct FInteractionData& Data);

private:
	// 여러 작업을 동시에 할 수 있을 경우를 대비해서 TArray로 제작
	UPROPERTY()
	TArray<TObjectPtr<UInteractionTask>> OnProgressTasks;
	UPROPERTY()
	TArray<TObjectPtr<UInteractionTask>> CompletedTasks;

	const uint8 bCanMultitask : 1 = false;
};
