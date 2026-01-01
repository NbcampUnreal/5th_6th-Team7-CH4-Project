// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/EquipmentComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "Alkaid/Character/AlkaidCharacter.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

}

void UEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEquipmentComponent, EquipmentType);
	DOREPLIFETIME(UEquipmentComponent, HeldItemRight);
	DOREPLIFETIME(UEquipmentComponent, HeldItemLeft);
}


void UEquipmentComponent::OnRep_EquipmentType()
{
	ApplyAttach();
}

void UEquipmentComponent::OnRep_HeldItems()
{
	ApplyAttach();
}

void UEquipmentComponent::ServerEquipRightItem_Implementation(AActor* Item, EEquipmentType NewType)
{
	if (!Item)
		return;

	if (HeldItemRight)
	{
		ServerDropItem();
	}
	HeldItemRight = Item;
	EquipmentType = NewType;
	ApplyHeldVisual(HeldItemRight, true);
	ApplyAttach();
}

void UEquipmentComponent::ServerEquipLeftItem_Implementation(AActor* Item, EEquipmentType NewType)
{
	if (!Item)
		return;
	if (HeldItemLeft)
	{
		HeldItemLeft->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		ApplyHeldVisual(HeldItemLeft, false);
		DropActorToFront(HeldItemLeft);
		HeldItemLeft = nullptr;
	}
	HeldItemLeft = Item;
	EquipmentType = NewType;
	ApplyHeldVisual(HeldItemLeft, true);
	ApplyAttach();
}

void UEquipmentComponent::ServerUnequipAllItems_Implementation()
{
	if (HeldItemLeft)
	{
		HeldItemLeft->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		ApplyHeldVisual(HeldItemLeft, false);
		DropActorToFront(HeldItemLeft);
		HeldItemLeft = nullptr;
	}
	if (HeldItemRight)
	{
		HeldItemRight->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		ApplyHeldVisual(HeldItemRight, false);
		DropActorToFront(HeldItemRight);
		HeldItemRight = nullptr;
	}
	EquipmentType = EEquipmentType::None;
	ApplyAttach();
}

void UEquipmentComponent::ServerTryInteract_Implementation()
{
	AAlkaidCharacter* OwnerAC = GetOwnerCharacter();
	if (!OwnerAC)
		return;

	UE_LOG(LogTemp, Warning, TEXT("[ServerTryInteract] Called. Owner=%s Role=%d HasAuth=%d"),
		*GetNameSafe(GetOwner()), (int32)GetOwner()->GetLocalRole(), GetOwner()->HasAuthority());


	FVector Start = OwnerAC->GetPawnViewLocation() + OwnerAC->GetControlRotation().Vector() * 30.0f;
	FVector End = Start + (OwnerAC->GetControlRotation().Vector() * 300.0f);

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(InteractTrace), false, OwnerAC);

	const float TraceRadius = 30.0f;

	const bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(TraceRadius),
		Params
	);
	UE_LOG(LogTemp, Warning, TEXT("[ServerTryInteract] Start=%s End=%s"),
		*Start.ToString(), *End.ToString());

	UE_LOG(LogTemp, Warning, TEXT("[ServerTryInteract] bHit=%d HitActor=%s HitComp=%s"),
		bHit,
		*GetNameSafe(Hit.GetActor()),
		*GetNameSafe(Hit.GetComponent()));

	AActor* Candidate = bHit ? Hit.GetActor() : nullptr;

	// 1) 맞은게 있고, 줍기 가능한 타입이면 -> 줍기
	if (Candidate)
	{
		const EEquipmentType NewType = DetermineEquipmentType(Candidate);
		if (NewType != EEquipmentType::None)
		{
			// 퍼즐이면 빈 손에 넣기(양손 운반)
			if (NewType == EEquipmentType::Puzzle)
			{
				if (!HeldItemRight) { ServerEquipRightItem(Candidate, NewType); return; }
				if (!HeldItemLeft) { ServerEquipLeftItem(Candidate, NewType);  return; }
				// 두 손 다 차 있으면 아무 것도 안 함
				return;
			}
			if (NewType == EEquipmentType::Block)
			{
				UE_LOG(LogTemp, Warning, TEXT("[Equip] Block interact. Candidate=%s"), *GetNameSafe(Candidate));

				if (OwnerAC->bIsPushing())
				{
					OwnerAC->ServerStopPushing();
					return;
				}

				if (HeldItemRight || HeldItemLeft)
				{
					UE_LOG(LogTemp, Warning, TEXT("[Equip] Block interact blocked: hands occupied. R=%s L=%s"),
						*GetNameSafe(HeldItemRight), *GetNameSafe(HeldItemLeft));
					return;
				}
				OwnerAC->ServerStartPushing(Candidate);
				UE_LOG(LogTemp, Warning, TEXT("[Equip] Sent ServerStartPushing"));
				return;
			}
			// 그 외는 한손(오른손) 정책
			if (HeldItemRight || HeldItemLeft) ServerDropItem();
			ServerEquipRightItem(Candidate, NewType);
			return;
		}
	}

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 3.0f, 0, 1.0f);
	DrawDebugSphere(GetWorld(), Start, TraceRadius, 12, FColor::Red, false, 3.0f);
	DrawDebugSphere(GetWorld(), End, TraceRadius, 12, FColor::Red, false, 3.0f);

	if (bHit)
	{
		DrawDebugSphere(GetWorld(), Hit.Location, TraceRadius, 12, FColor::Green, false, 3.0f);
	}
}

void UEquipmentComponent::ItemDrop(TObjectPtr<AActor>& ItemSlot)
{
	if (!ItemSlot)
		return;

	AAlkaidCharacter* OwnerAC = GetOwnerCharacter();
	if (!OwnerAC)
		return;

	AActor* Item = ItemSlot;

	Item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	const FVector DropLocation = OwnerAC->GetActorLocation() + OwnerAC->GetActorForwardVector() * 90.0f
		+ FVector(0.f, 0.f, 40.f);

	Item->SetActorLocation(DropLocation);

	if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Item->GetRootComponent()))
	{
		PrimComp->SetSimulatePhysics(true);
		PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	ItemSlot = nullptr;
}

void UEquipmentComponent::RequestInteractToggle(AActor*)
{

	if (!GetOwner())
		return;

	if (!GetOwner()->HasAuthority())
	{

		ServerTryInteract();
		return;
	}


	ServerTryInteract();
}

void UEquipmentComponent::ServerDropItem_Implementation()
{
	if (HeldItemRight)
	{
		ItemDrop(HeldItemRight);
		if (!HeldItemRight) EquipmentType = EEquipmentType::None;
		ApplyAttach();
		return;
	}

	if (HeldItemLeft)
	{
		ItemDrop(HeldItemLeft);
		if (!HeldItemLeft) EquipmentType = EEquipmentType::None;
		ApplyAttach();
		return;
	}
}

void UEquipmentComponent::ServerToggleInteract_Implementation(AActor* CandidateItem)
{
	if (HeldItemRight)
	{
		ServerDropItem();
		return;
	}

	if (HeldItemLeft)
	{
		ServerDropItem();
		return;
	}

	if (!CandidateItem)
		return;

	const EEquipmentType NewType = DetermineEquipmentType(CandidateItem);
	if (NewType == EEquipmentType::None)
		return;

	ServerEquipRightItem(CandidateItem, NewType);
}

void UEquipmentComponent::UsingItemInputStarted()
{
	if (!GetOwner())
		return;

	bUsingItemPressed = true;
	UsingItemPressedTime = GetWorld()->GetTimeSeconds();
}

void UEquipmentComponent::UsingItemInputCompleted()
{
	if (!bUsingItemPressed) return;
	bUsingItemPressed = false;

	AAlkaidCharacter* OwnerAC = GetOwnerCharacter();
	if (!OwnerAC) return;

	const float HeldFor = GetWorld()->GetTimeSeconds() - UsingItemPressedTime;

	// 1) 푸시 중이면: 길게/짧게 상관없이 "Stop" 우선
	if (OwnerAC->bIsPushing())
	{
		OwnerAC->ServerStopPushing();
		return;
	}

	// 2) 길게 누르면: 들고있는 아이템 드랍
	if (HeldFor >= DropHoldSeconds)
	{
		ServerDropItem();
		return;
	}

	// 3) 짧게 누르면: 상호작용(줍기)
	RequestInteractToggle(nullptr);
}

void UEquipmentComponent::UsingItemInputCanceled()
{
	bUsingItemPressed = false;
}

// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	ApplyAttach();
}

void UEquipmentComponent::ApplyAttach()
{
	AAlkaidCharacter* OwnerAC = GetOwnerCharacter();
	if (!OwnerAC)
		return;

	USkeletalMeshComponent* MeshComp = OwnerAC->GetMesh();
	if (!MeshComp)
		return;

	if (HeldItemRight)
	{
		HeldItemRight->AttachToComponent(
			MeshComp,
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			RightHandSocket
		);
		UStaticMeshComponent* ItemMesh= HeldItemRight->FindComponentByClass<UStaticMeshComponent>();

		if (ItemMesh != nullptr)
		{
			ItemMesh->SetSimulatePhysics(false);
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	if (HeldItemLeft)
	{
		HeldItemLeft->AttachToComponent(
			MeshComp,
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			LeftHandSocket
		);
	}
}

void UEquipmentComponent::ApplyHeldVisual(AActor* Item, bool bHeld)
{
	if (!Item)
		return;

	if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Item->GetRootComponent()))
	{
		if (bHeld)
		{
			PrimComp->SetSimulatePhysics(false);
			PrimComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		else
		{
			PrimComp->SetSimulatePhysics(true);
			PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
	}
}


void UEquipmentComponent::DropActorToFront(AActor* Item)
{
	AAlkaidCharacter* OwnerAC = GetOwnerCharacter();
	if (!OwnerAC || !Item)
		return;

	const FVector DropLocation = OwnerAC->GetActorLocation() + OwnerAC->GetActorForwardVector() * 90.0f
		+ FVector(0.f, 0.f, 40.f);

	Item->SetActorLocation(DropLocation);
}

AAlkaidCharacter* UEquipmentComponent::GetOwnerCharacter() const
{
	return Cast<AAlkaidCharacter>(GetOwner());
}

EEquipmentType UEquipmentComponent::DetermineEquipmentType(AActor* Item) const
{
	if (!Item) return EEquipmentType::None;

	UE_LOG(LogTemp, Warning, TEXT("[Equip] DetermineEquipmentType Item=%s Class=%s"),
		*Item->GetName(), *Item->GetClass()->GetName());


	for (const auto& Pair : ItemClassToEquipmentType)
	{
		if (Pair.Key)
		{
			UE_LOG(LogTemp, Warning, TEXT("  - Check Key=%s  IsA=%d"),
				*Pair.Key->GetName(), Item->IsA(Pair.Key));
		}

		if (Pair.Key && Item->IsA(Pair.Key))
		{
			UE_LOG(LogTemp, Warning, TEXT("  => MATCH Type=%d"), (int32)Pair.Value);
			return Pair.Value;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("  => NO MATCH"));
	return EEquipmentType::None;
}

// Called every frame
void UEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

