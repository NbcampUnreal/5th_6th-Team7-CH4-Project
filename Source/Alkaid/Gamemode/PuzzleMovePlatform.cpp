#include "Gamemode/PuzzleMovePlatform.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

APuzzleMovePlatform::APuzzleMovePlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	SetReplicateMovement(true);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
}

void APuzzleMovePlatform::BeginPlay()
{
	Super::BeginPlay();
	MoveDir = 0;
}

void APuzzleMovePlatform::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (MoveDir == 0)
	{
		return;
	}

	FVector Loc = GetActorLocation();
	const float TargetY = (MoveDir < 0) ? MinY : MaxY;

	Loc.Y = FMath::FInterpConstantTo(Loc.Y, TargetY, DeltaSeconds, MoveSpeed);
	SetActorLocation(Loc);

	if (FMath::IsNearlyEqual(Loc.Y, TargetY, 0.5f))
	{
		MoveDir = 0;
	}
}

void APuzzleMovePlatform::StartMoveLeft()
{
	if (!HasAuthority()) return;
	MoveDir = -1;
}

void APuzzleMovePlatform::StartMoveRight()
{
	if (!HasAuthority()) return;
	MoveDir = +1;
}

void APuzzleMovePlatform::StopMove()
{
	if (!HasAuthority()) return;
	MoveDir = 0;
}