#include "Gamemode/PuzzlePieceBase.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

APuzzlePieceBase::APuzzlePieceBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);
	NetUpdateFrequency = 60.f;
	MinNetUpdateFrequency = 30.f;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	Mesh->SetIsReplicated(true);
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	Mesh->SetGenerateOverlapEvents(true);

	// 생성자에서는 기본값만, 실제 시뮬 ON/OFF는 BeginPlay에서 권한으로 결정
	Mesh->SetSimulatePhysics(false);
	Mesh->SetEnableGravity(false);
}

void APuzzlePieceBase::BeginPlay()
{
	Super::BeginPlay();

	// 물리/중력은 서버처리
	if (HasAuthority())
	{
		Mesh->SetSimulatePhysics(true);
		Mesh->SetEnableGravity(true);
	}
	else
	{
		// 클라는 서버 물리 결과를 복제
		Mesh->SetSimulatePhysics(false);
		Mesh->SetEnableGravity(false);
	}
}

void APuzzlePieceBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APuzzlePieceBase, PieceId);
}