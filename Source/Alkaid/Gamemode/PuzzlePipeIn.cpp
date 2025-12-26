#include "Gamemode/PuzzlePipeIn.h"

#include "Gamemode/PuzzlePipeOut.h"
#include "Gamemode/PuzzlePieceBase.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

APuzzlePipeIn::APuzzlePipeIn()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(Root);

	TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("TriggerCapsule"));
	TriggerCapsule->SetupAttachment(Root);

	// 트리커 기본 크기(레벨/BP에서 조절가능)
	TriggerBox->SetBoxExtent(FVector(60.f, 60.f, 60.f));
	TriggerCapsule->SetCapsuleSize(60.f, 60.f);

	// 선택 트리거 지정
	ResolveTriggerComponent();
	if (Trigger)
	{
		ApplyTriggerCollisionDefaults(Trigger);
		Trigger->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnTriggerBeginOverlap);
	}
}

void APuzzlePipeIn::BeginPlay()
{
	Super::BeginPlay();

	ResolveTriggerComponent();

	if (!Trigger)
	{
		UE_LOG(LogTemp, Warning, TEXT("PuzzlePipeIn: Trigger(Box/Capsule) not found."));
		return;
	}

	ApplyTriggerCollisionDefaults(Trigger);

	// 중복 바인딩 방지
	Trigger->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnTriggerBeginOverlap);
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnTriggerBeginOverlap);
}

void APuzzlePipeIn::ResolveTriggerComponent()
{
	Trigger = nullptr;

	// 트리거 두가지 설정
	if (TriggerBox)     TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (TriggerCapsule) TriggerCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 선택된 것 켜기
	if (TriggerShape == EPuzzleTriggerShape::Box)
	{
		if (TriggerBox)
		{
			Trigger = TriggerBox;
		}
	}
	else
	{
		if (TriggerCapsule)
		{
			Trigger = TriggerCapsule;
		}
	}

	// 폴백
	if (!Trigger)
	{
		if (TriggerCapsule) Trigger = TriggerCapsule;
		else if (TriggerBox) Trigger = TriggerBox;
	}
}

void APuzzlePipeIn::ApplyTriggerCollisionDefaults(UShapeComponent* InTrigger)
{
	if (!InTrigger) return;

	InTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	InTrigger->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	InTrigger->SetGenerateOverlapEvents(true);
}

void APuzzlePipeIn::OnTriggerBeginOverlap(
	UPrimitiveComponent*, AActor* OtherActor,
	UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	if (!HasAuthority())
	{
		return;
	}

	APuzzlePieceBase* Piece = Cast<APuzzlePieceBase>(OtherActor);
	if (!IsValid(Piece))
	{
		return;
	}

	if (!IsValid(LinkedOutPipe))
	{
		UE_LOG(LogTemp, Warning, TEXT("PuzzlePipeIn: LinkedOutPipe is not set."));
		return;
	}

	// 조각 정보 저장
	const FPuzzleId SavedId = Piece->PieceId;
	TSubclassOf<APuzzlePieceBase> PieceClass = Piece->GetClass();

	// 기존 조각 제거
	Piece->Destroy();

	// Out에서 생성(out에서 생성후 중력으로 낙하)
	LinkedOutPipe->SpawnPiece(PieceClass, SavedId);
}