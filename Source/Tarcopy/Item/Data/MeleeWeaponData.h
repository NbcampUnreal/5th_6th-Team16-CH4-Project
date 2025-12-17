#pragma once

#include "CoreMinimal.h"
#include "Item/ItemEnums.h"
#include "MeleeWeaponData.generated.h"

USTRUCT()
struct TARCOPY_API FMeleeWeaponData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EBodyLocation BodyLocation = EBodyLocation::Hands;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DoorDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TreeDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinRange;												// 무기 공격이 밀치기로 전환되는 최소 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxRange;												// 무기 최대 데미지가 적용되는 타점이자 최대 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackSpeed = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CritChance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CritMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Knockback;
};
