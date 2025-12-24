#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

// test
#include "Misc/Guid.h"

#include "ItemInstance.generated.h"

enum class EItemComponent : uint8;
struct FItemData;
class UItemComponentBase;

DECLARE_MULTICAST_DELEGATE(FOnItemUpdated)

UCLASS(BlueprintType)
class TARCOPY_API UItemInstance : public UObject
{
	GENERATED_BODY()

public:
	virtual bool IsSupportedForNetworking() const override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack) override;

public:
	void SetItemId(const FName& InItemId);
	const FItemData* GetData() const { return Data; }

	template <typename T>
	T* GetItemComponent() const
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

	const TArray<TObjectPtr<UItemComponentBase>>& GetItemComponents() const;

	void SetOwnerCharacter(ACharacter* InOwnerCharacter);
	ACharacter* GetOwnerCharacter() const { return OwnerCharacter.IsValid() == true ? OwnerCharacter.Get() : nullptr; };

	bool HasAuthority() const;

	void CancelAllComponentActions();

protected:
	UFUNCTION()
	void OnRep_SetData();
	UFUNCTION()
	void OnRep_ItemUpdated();
	UFUNCTION()
	void OnRep_SetOwnerCharacter();

	void InitComponents();

public:
	FOnItemUpdated OnItemUpdated;

protected:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_ItemUpdated)
	TArray<TObjectPtr<UItemComponentBase>> ItemComponents;

	UPROPERTY(ReplicatedUsing = OnRep_SetData)
	FName ItemId;

	UPROPERTY(ReplicatedUsing = OnRep_SetOwnerCharacter)
	TWeakObjectPtr<ACharacter> OwnerCharacter;

	const FItemData* Data = nullptr;

//test
public:
	UPROPERTY(Replicated)
	FGuid InstanceID;
	FGuid GetInstanceId() { return InstanceID; }
};
