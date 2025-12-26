// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/EquipmentComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/PrimitiveComponent.h"
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
	if(!Item)
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
	if(!Item)
		return;
	if(HeldItemLeft)
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
	if(HeldItemLeft)
	{
		HeldItemLeft->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		ApplyHeldVisual(HeldItemLeft, false);
		DropActorToFront(HeldItemLeft);
		HeldItemLeft = nullptr;
	}
	if(HeldItemRight)
	{
		HeldItemRight->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		ApplyHeldVisual(HeldItemRight, false);
		DropActorToFront(HeldItemRight);
		HeldItemRight = nullptr;
	}
	EquipmentType = EEquipmentType::None;
	ApplyAttach();
}

void UEquipmentComponent::ItemDrop(TObjectPtr<AActor>& ItemSlot)
{
	if(!ItemSlot)
		return;

	AAlkaidCharacter* OwnerAC = GetOwnerCharacter();
	if (!OwnerAC)
		return;

	AActor* Item = ItemSlot;

	Item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	const FVector DropLocation = OwnerAC->GetActorLocation() + OwnerAC->GetActorForwardVector() * 90.0f
		+ FVector(0.f, 0.f, 40.f);

	Item->SetActorLocation(DropLocation);

	if(UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Item->GetRootComponent()))
	{
		PrimComp->SetSimulatePhysics(true);
		PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	ItemSlot = nullptr;
}

void UEquipmentComponent::RequestInteractToggle(AActor* CandidateItem)
{
	if (!GetOwner())
		return;

	if (!GetOwner()->HasAuthority())
	{
		ServerToggleInteract(CandidateItem);
		return;
	}
	ServerToggleInteract(CandidateItem);
}

void UEquipmentComponent::ServerDropItem_Implementation()
{
	if (!HeldItemRight && !HeldItemLeft)
		return;

	ItemDrop(HeldItemRight);
	ItemDrop(HeldItemLeft);

	EquipmentType = EEquipmentType::None;

	ApplyAttach();
}

void UEquipmentComponent::ServerToggleInteract_Implementation(AActor* CandidateItem)
{
	if (HeldItemRight)
	{
		ServerDropItem();
		return;
	}

	if(HeldItemLeft)
	{
		ServerDropItem();
		return;
	}

	if(!CandidateItem)
		return;

	const EEquipmentType NewType = DetermineEquipmentType(CandidateItem);
	if(NewType == EEquipmentType::None)
		return;

	ServerEquipRightItem(CandidateItem, NewType);
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
	if(!OwnerAC) 
		return;

	USkeletalMeshComponent* MeshComp = OwnerAC->GetMesh();
	if(!MeshComp)
		return;

	if (HeldItemRight)
	{
		HeldItemRight->AttachToComponent(
			MeshComp,
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			RightHandSocket
		);
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

	for(const auto& Pair : ItemClassToEquipmentType)
	{
		if (Pair.Key &&Item->IsA(Pair.Key))
		{
			return Pair.Value;
		}
	}
	return EEquipmentType::None;
}

// Called every frame
void UEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

