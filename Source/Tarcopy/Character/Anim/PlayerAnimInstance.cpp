#include "Character/Anim/PlayerAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/Anim/AnimationPreset.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ACharacter>(GetOwningActor());
	if (IsValid(Character))
	{
		MovementComponent = Character->GetCharacterMovement();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!IsValid(Character)) return;

	Velocity = MovementComponent->Velocity;
	GroundSpeed = Velocity.Size2D();

	bShouldMove = (MovementComponent->GetCurrentAcceleration().IsNearlyZero() == false) && (GroundSpeed > 3.f);
	bIsFalling = MovementComponent->IsFalling();
}
