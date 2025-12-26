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

void APuzzleSignBase::SetDisplayId(FName NewId)
{
	if (!HasAuthority())
	{
		return;
	}

	DisplayId.Id = NewId;

	// 서버에서도 바로 외형 반영되게 호출
	BP_OnDisplayIdChanged(DisplayId.Id);
}

void APuzzleSignBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APuzzleSignBase, DisplayId);
}

void APuzzleSignBase::ServerSetDisplayId(const FPuzzleId& NewId)
{
	if (!HasAuthority())
	{
		return;
	}

	DisplayId = NewId;
	BP_OnDisplayIdChanged(DisplayId.Id); // 서버 즉시 반영
}