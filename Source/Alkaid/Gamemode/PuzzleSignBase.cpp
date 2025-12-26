#include "Gamemode/PuzzleSignBase.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

APuzzleSignBase::APuzzleSignBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APuzzleSignBase::OnRep_DisplayId()
{
	BP_OnDisplayIdChanged(DisplayId.Id);
}

void APuzzleSignBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APuzzleSignBase, DisplayId);
}