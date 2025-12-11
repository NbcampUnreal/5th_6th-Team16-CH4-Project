#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Item/Data/InteractionData.h"
#include "InteractionTask.generated.h"

DECLARE_DELEGATE_OneParam(FOnCompleted, UInteractionTask*)

UCLASS(Abstract)
class TARCOPY_API UInteractionTask : public UObject
{
	GENERATED_BODY()
	
public:
	void Init(AActor* InInstigator, const FInteractionData& InData);
	void OnProgressInteraction(float DeltaTime);

protected:
	virtual void OnInteractionCompleted();
	void ReduceDurability();

public:
	FOnCompleted OnCompleted;

protected:
	UPROPERTY()
	TWeakObjectPtr<AActor> Instigator;
	UPROPERTY()
	FInteractionData Data;

	float Duration = 0.0f;
	float ElapsedTime = 0.0f;
};
