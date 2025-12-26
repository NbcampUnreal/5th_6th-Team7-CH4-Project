#include "Gamemode/PuzzlePipeOut.h"
#include "Gamemode/PuzzlePieceBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"

APuzzlePipeOut::APuzzlePipeOut()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	SpawnPoint->SetupAttachment(Root);
}

APuzzlePieceBase* APuzzlePipeOut::SpawnPiece(TSubclassOf<APuzzlePieceBase> PieceClass, FPuzzleId InPieceId)
{
	if (!HasAuthority())
	{
		return nullptr;
	}
	if (!PieceClass)
	{
		return nullptr;
	}

	const FTransform SpawnTM = SpawnPoint ? SpawnPoint->GetComponentTransform() : GetActorTransform();

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APuzzlePieceBase* Spawned = GetWorld()->SpawnActor<APuzzlePieceBase>(PieceClass, SpawnTM, Params);
	if (!IsValid(Spawned))
	{
		return nullptr;
	}

	Spawned->PieceId = InPieceId;

	// 스폰 직후, 물리 on(강제시켜 오류방지)
	if (Spawned->Mesh)
	{
		Spawned->Mesh->WakeAllRigidBodies();
	}

	return Spawned;
}