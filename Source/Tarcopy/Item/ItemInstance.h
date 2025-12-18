#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

// test
#include "Misc/Guid.h"

#include "ItemInstance.generated.h"

enum class EItemComponent : uint8;
struct FItemData;
class UItemComponentBase;

UCLASS(BlueprintType)
class TARCOPY_API UItemInstance : public UObject
{
	GENERATED_BODY()

public:
	void SetData(const FItemData* InData);
	const FItemData* GetData() const { return Data; }

	template <typename T>
	const T* GetItemComponent() const
	{
		static_assert(TIsDerivedFrom<T, UItemComponentBase>::IsDerived, "T must be derived from UItemComponentBase");

		for (const auto& Component : ItemComponents)
		{
			if (T* CastedComponent = Cast<T>(Component.Get()))
			{
				return CastedComponent;
			}
		}

		return nullptr;
	}

	const TArray<TObjectPtr<UItemComponentBase>> GetItemComponents() const;

protected:
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UItemComponentBase>> ItemComponents;

	const FItemData* Data = nullptr;

//test
public:
	FGuid InstanceID;
	FGuid GetInstanceId() { return InstanceID; }
};
