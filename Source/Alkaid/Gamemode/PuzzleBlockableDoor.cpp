#include "Gamemode/PuzzleBlockableDoor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

APuzzleBlockableDoor::APuzzleBlockableDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	// 액터의 위치를 움직이는 게 아니라 "패널 컴포넌트"를 움직임
	// ReplicateMovement는 굳이 필요 없음(Offset을 RepNotify로 보냄).
	SetReplicateMovement(false);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PanelL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PanelL"));
	PanelL->SetupAttachment(Root);

	PanelR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PanelR"));
	PanelR->SetupAttachment(Root);

	// 문 패널이 오브젝트에 "막혀야" 하므로 기본적으로 Block 충돌이 필요
	// (충돌 프리셋은 BP에서 BlockAllDynamic)
	PanelL->SetGenerateOverlapEvents(false);
	PanelR->SetGenerateOverlapEvents(false);
}

// 닫힘 기준 상대좌표를 "한 번만" 저장(에디터/블프에서 배치한 값 그대로)
void APuzzleBlockableDoor::CacheClosedRelativeIfNeeded()
{
	if (bClosedRelCached) return;
	if (!IsValid(PanelL) || !IsValid(PanelR)) return;

	ClosedRelL = PanelL->GetRelativeLocation();
	ClosedRelR = PanelR->GetRelativeLocation();
	bClosedRelCached = true;
}

void APuzzleBlockableDoor::BeginPlay()
{
	Super::BeginPlay();

	CacheClosedRelativeIfNeeded();
	ApplyOffset(CurrentOffset);
}

void APuzzleBlockableDoor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority())
	{
		return;
	}

	ServerStepMove(DeltaSeconds);
}

void APuzzleBlockableDoor::SetActive(bool bInActive)
{
	if (!HasAuthority())
	{
		return;
	}
	bActive = bInActive;
}

void APuzzleBlockableDoor::ServerStepMove(float DeltaSeconds)
{
	const float Target = bActive ? OpenDistance : 0.f;

	// 이미 목표에 가까우면 종료
	if (FMath::IsNearlyEqual(CurrentOffset, Target, 0.5f))
	{
		CurrentOffset = Target;
		ApplyOffset(CurrentOffset);
		return;
	}

	// 이번 틱에 이동할 오프셋 변화량
	const float Next = FMath::FInterpConstantTo(CurrentOffset, Target, DeltaSeconds, MoveSpeed);
	const float DeltaOffset = Next - CurrentOffset;

	// 패널 2개가 반씩 움직임
	const float HalfDelta = DeltaOffset * 0.5f;

	// 상대좌표 기준: L은 -방향, R은 +방향으로 벌어지게 설계
	const FVector DeltaL(0.f, -HalfDelta, 0.f);
	const FVector DeltaR(0.f, +HalfDelta, 0.f);

	// 닫히는 중(DeltaOffset < 0)일 때 오브젝트에 막히면 "그 자리에서 멈춤"
	const bool bClosing = (DeltaOffset < 0.f);

	FHitResult HitL, HitR;

	if (IsValid(PanelL))
	{
		PanelL->MoveComponent(DeltaL, PanelL->GetComponentQuat(), /*bSweep*/ bClosing, &HitL);
	}
	if (IsValid(PanelR))
	{
		PanelR->MoveComponent(DeltaR, PanelR->GetComponentQuat(), /*bSweep*/ bClosing, &HitR);
	}

	// 막혔으면(닫히는 중이고 Hit 발생) 오프셋을 "실제 패널 위치"로 재계산하고 더 닫지 않음
	if (bClosing && (HitL.bBlockingHit || HitR.bBlockingHit))
	{
		CurrentOffset = CalcOffsetFromPanels();
		// bActive는 이미 false일 텐데, 닫힘은 물체가 빠질 때까지 진행 못 하므로 여기서 멈춤
		return;
	}

	// 정상 이동이면 Next로 갱신
	CurrentOffset = FMath::Clamp(Next, 0.f, OpenDistance);
	ApplyOffset(CurrentOffset);
}

void APuzzleBlockableDoor::ApplyOffset(float InOffset)
{
	if (!IsValid(PanelL) || !IsValid(PanelR))
	{
		return;
	}

	CacheClosedRelativeIfNeeded();

	InOffset = FMath::Clamp(InOffset, 0.f, OpenDistance);

	// 두 패널 간 간격이 InOffset이 되도록 배치
	const float Half = InOffset * 0.5f;

	//"닫힘 기준(ClosedRel) + 델타"

	FVector L = ClosedRelL;
	FVector R = ClosedRelR;

	L.Y += -Half;
	R.Y += +Half;

	PanelL->SetRelativeLocation(L);
	PanelR->SetRelativeLocation(R);
}

float APuzzleBlockableDoor::CalcOffsetFromPanels() const
{
	if (!IsValid(PanelL) || !IsValid(PanelR))
	{
		return CurrentOffset;
	}
	// 닫힘 기준 위치 대비 얼마나 이동했는지로 Offset 재계산 
	const float DeltaL = PanelL->GetRelativeLocation().Y - ClosedRelL.Y; 
	const float DeltaR = PanelR->GetRelativeLocation().Y - ClosedRelR.Y; 
	const float Offset = DeltaR - DeltaL; //수정 (결과적으로 InOffset에 해당)
	return FMath::Clamp(Offset, 0.f, OpenDistance);
}

void APuzzleBlockableDoor::OnRep_CurrentOffset()
{
	ApplyOffset(CurrentOffset);
}

void APuzzleBlockableDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APuzzleBlockableDoor, CurrentOffset);
	DOREPLIFETIME(APuzzleBlockableDoor, bActive);
}