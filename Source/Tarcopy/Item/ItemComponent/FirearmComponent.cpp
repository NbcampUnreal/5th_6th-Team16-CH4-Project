#include "Item/ItemComponent/FirearmComponent.h"
#include "Item/DataTableSubsystem.h"
#include "Item/Data/FirearmData.h"
#include "Item/ItemInstance.h"
#include "Item/Data/ItemData.h"
#include "Character/MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"
#include "Tarcopy.h"
#include "Kismet/GameplayStatics.h"
#include "Item/ItemCommand/EquipCommand.h"
#include "Item/ItemComponent/DurabilityComponent.h"

void UFirearmComponent::SetOwnerItem(UItemInstance* InOwnerItem)
{
	Super::SetOwnerItem(InOwnerItem);

	if (Data == nullptr)
		return;
}

void UFirearmComponent::GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands)
{
	Super::GetCommands(OutCommands);

	const FItemData* OwnerItemData = GetOwnerItemData();
	checkf(OwnerItemData != nullptr, TEXT("Owner Item has No Data"));
	FText TextItemName = OwnerItemData->TextName;

	bool bIsEquipped = IsValid(GetOwnerCharacter()) == true;
	UEquipCommand* EquipCommand = NewObject<UEquipCommand>(this);
	EquipCommand->TargetItem = GetOwnerItem();
	EquipCommand->TextDisplay = FText::Format(
		FText::FromString(bIsEquipped == true ? TEXT("Unequip {0}") : TEXT("Equip {0}")),
		TextItemName);
	EquipCommand->bEquip = !bIsEquipped;
	EquipCommand->BodyLocation = Data->BodyLocation;
	// 인벤토리에서 공간 있는지 체크해야 함
	EquipCommand->bExecutable = true;
	OutCommands.Add(EquipCommand);
}

void UFirearmComponent::SetOwnerHoldingItemMesh()
{
	if (Data == nullptr)
		return;

	SetOwnerHoldingItemMeshAtSocket(Data->Socket);
}

void UFirearmComponent::OnExecuteAttack()
{
	if (Data == nullptr)
		return;

	AMyCharacter* MyCharacter = Cast<AMyCharacter>(GetOwnerCharacter());
	if (IsValid(MyCharacter) == false)
		return;

	UCharacterMovementComponent* CharacterMovement = MyCharacter->GetCharacterMovement();
	if (IsValid(CharacterMovement) == false)
		return;

	bIsAttacking = true;

	NetMulticast_PlayAttackMontage();

	float AttackDuration = IsValid(Data->Montage) == true ? Data->Montage->GetPlayLength() : 1.0f;
	CharacterMovement->DisableMovement();
	MyCharacter->GetWorldTimerManager().SetTimer(
		EnableMovementTimerHandle,
		this,
		&ThisClass::EnableOwnerMovement,
		AttackDuration,
		false);

	FHitResult HitResult;
	AActor* AimTarget = nullptr;
	FName BoneName = NAME_None;
	// 마우스 포인터가 적을 포인팅 중이면, 현재 가리키고 있는 적의 Bone으로 공격
	// 아니라면, 그냥 Muzzle에서 수평하게 마우스 포인터 방향으로 공격
	bool bHit = MyCharacter->GetAimTarget(AimTarget, BoneName);

	FVector StartLocation = MyCharacter->GetActorLocation();		// 액터 Muzzle 소켓으로 바꿔야 함
	FVector TargetDir = MyCharacter->GetActorForwardVector();

	if (bHit == true)
	{
		USkeletalMeshComponent* TargetSkeletal = AimTarget->FindComponentByClass<USkeletalMeshComponent>();
		if (IsValid(TargetSkeletal) == true && TargetSkeletal->DoesSocketExist(BoneName) == true)
		{
			FVector TargetLocation = TargetSkeletal->GetSocketLocation(BoneName);
			TargetDir = (TargetLocation - StartLocation).GetSafeNormal();
		}
	}

	FVector EndLocation = StartLocation + TargetDir * Data->MaxRange;
	CheckHit(StartLocation, EndLocation);
}

void UFirearmComponent::CancelAction()
{
	UWorld* World = GetWorld();
	if (IsValid(World) == true)
	{
		World->GetTimerManager().ClearTimer(EnableMovementTimerHandle);
	}

	NetMulticast_StopAttackMontage();
	EnableOwnerMovement();
}

void UFirearmComponent::OnRep_SetComponent()
{
	const FItemData* ItemData = GetOwnerItemData();
	if (ItemData == nullptr)
		return;

	UDataTableSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	if (IsValid(DataTableSubsystem) == false)
		return;

	Data = DataTableSubsystem->GetTable(EDataTableType::FirearmTable)->FindRow<FFirearmData>(ItemData->ItemId, FString(""));
}

void UFirearmComponent::CheckHit(const FVector& StartLocation, const FVector& EndLocation)
{
	if (Data == nullptr)
		return;

	ACharacter* OwnerCharacter = GetOwnerCharacter();
	if (IsValid(OwnerCharacter) == false)
		return;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);
	Params.bTraceComplex = true;
	// 적 캡슐 Visibility 끄고 나머진 켜기
	FCollisionShape SphereCollision = FCollisionShape::MakeSphere(10.0f);
	bool bHit = GetWorld()->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity, ECC_Visibility, SphereCollision, Params);
	FVector ActualEndLocation = bHit == true ? HitResult.ImpactPoint : EndLocation;

	// 나이아가라 트레일 연출

	float MinRangeSquared = FMath::Square(Data->MinRange);
	float MaxRangeSquared = FMath::Square(Data->MaxRange);

	if (bHit == true)
	{
		AActor* HitActor = HitResult.GetActor();
		if (IsValid(HitActor) == false)
			return;

		float DistSquared = FVector::DistSquared(StartLocation, ActualEndLocation);
		float Damage = FMath::GetMappedRangeValueClamped(
			FVector2D(MinRangeSquared, MaxRangeSquared),
			FVector2D(Data->MinDamage, Data->MaxDamage),
			DistSquared);

		FVector Dir = (ActualEndLocation - StartLocation).GetSafeNormal();
		UGameplayStatics::ApplyPointDamage(
			HitActor,
			Damage,
			Dir,
			HitResult,
			OwnerCharacter->GetController(),
			OwnerCharacter,
			nullptr);

		UDurabilityComponent* DurabilityComponent = GetOwnerItem()->GetItemComponent<UDurabilityComponent>();
		if (IsValid(DurabilityComponent) == true)
		{
			// 바꿔야 함
			float TempAmount = 1.0;
			DurabilityComponent->LoseDurability(TempAmount);
		}
	}

	// 충돌 검사 디버그
	{
		FVector TraceVec = ActualEndLocation - StartLocation;
		float TraceLen = TraceVec.Size();
		FVector Center = StartLocation + TraceVec * 0.5f;
		FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();

		DrawDebugCapsule(
			GetWorld(),
			Center,
			(TraceLen * 0.5f) + 10.0f,
			10.0f,
			CapsuleRot,
			bHit == true ? FColor::Green : FColor::Red,
			false,
			5.0f);
	}
}

void UFirearmComponent::NetMulticast_PlayAttackMontage_Implementation()
{
	ACharacter* OwnerCharacter = GetOwnerCharacter();
	if (IsValid(OwnerCharacter) == false)
		return;

	if (IsValid(Data->Montage) == false)
		return;

	OwnerCharacter->PlayAnimMontage(Data->Montage);
}

void UFirearmComponent::NetMulticast_StopAttackMontage_Implementation()
{
	ACharacter* OwnerCharacter = GetOwnerCharacter();
	if (IsValid(OwnerCharacter) == false)
		return;

	if (IsValid(Data->Montage) == false)
		return;

	OwnerCharacter->StopAnimMontage(Data->Montage);
}
