#include "Gamemode/PuzzleMoveHoleWall.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

APuzzleMoveHoleWall::APuzzleMoveHoleWall()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false); // 평소엔 꺼두고, 움직일 때만 켬

	bReplicates = true;
	SetReplicateMovement(true);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void APuzzleMoveHoleWall::BeginPlay()
{
	Super::BeginPlay();
	ApplyTickEnabledFromDir();
}

void APuzzleMoveHoleWall::StartMoveLeft()
{
	if (!HasAuthority()) return;
	MoveDir = -1;
	ApplyTickEnabledFromDir();
	ForceNetUpdate();
}

void APuzzleMoveHoleWall::StartMoveRight()
{
	if (!HasAuthority()) return;
	MoveDir = +1;
	ApplyTickEnabledFromDir();
	ForceNetUpdate();
}

void APuzzleMoveHoleWall::StopMove()
{
	if (!HasAuthority()) return;
	MoveDir = 0;
	ApplyTickEnabledFromDir();
	ForceNetUpdate();
}

void APuzzleMoveHoleWall::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority()) return;
	if (MoveDir == 0) return;

	FVector Loc = GetActorLocation();

	// Y만 이동
	Loc.Y += (float)MoveDir * MoveSpeed * DeltaSeconds;

	// 범위 제한
	const float ClampedY = FMath::Clamp(Loc.Y, MinY, MaxY);
	const bool bHitLimit = !FMath::IsNearlyEqual(Loc.Y, ClampedY, 0.1f);
	Loc.Y = ClampedY;

	SetActorLocation(Loc, false, nullptr, ETeleportType::None);

	// 끝에 닿으면 자동 정지
	if (bHitLimit)
	{
		StopMove();
	}
}

void APuzzleMoveHoleWall::OnRep_MoveDir()
{
	ApplyTickEnabledFromDir();
}

void APuzzleMoveHoleWall::ApplyTickEnabledFromDir()
{
	// 서버는 이동 중일 때만 Tick
	if (HasAuthority())
	{
		SetActorTickEnabled(MoveDir != 0);
	}
	else
	{
		// 클라는 서버 위치 복제만 받으면 되므로 Tick 꺼도 OK
		SetActorTickEnabled(false);
	}
}

void APuzzleMoveHoleWall::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APuzzleMoveHoleWall, MoveDir);
}