#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AnimationPreset.generated.h"

UCLASS()
class TARCOPY_API UAnimationPreset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimInstance> IdleAnimation;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimInstance> WalkAnimation;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimInstance> SprintAnimation;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimInstance> CrouchAnimation;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimInstance> HitAnimation;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimInstance> AimAnimation;
};
