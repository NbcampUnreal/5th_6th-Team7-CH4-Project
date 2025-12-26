#include "Gamemode/PuzzleReceiverBox.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ShapeComponent.h"
#include "Net/UnrealNetwork.h"
#include "Gamemode/PuzzlePieceBase.h"

APuzzleReceiverBox::APuzzleReceiverBox()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(Root);

	TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("TriggerCapsule"));
	TriggerCapsule->SetupAttachment(Root);

	// 트리거 크기(BP에서 수정)
	TriggerBox->SetBoxExtent(FVector(60.f, 60.f, 60.f));
	TriggerCapsule->SetCapsuleSize(60.f, 80.f);

	// 둘 다 기본은 꺼두고 Resolve에서 하나만 켭니다
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TriggerCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerCapsule->SetGenerateOverlapEvents(true);

	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);

	TriggerBox->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	TriggerCapsule->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
}

void APuzzleReceiverBox::BeginPlay()
{
	Super::BeginPlay();

	ResolveTriggerComponent();
	if (!Trigger)
	{
		UE_LOG(LogTemp, Warning, TEXT("PuzzleReceiverBox: Trigger not found."));
		return;
	}

	// 서버에서 판정
	if (HasAuthority())
	{
		Trigger->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnTriggerBeginOverlap);
	}
}

void APuzzleReceiverBox::ResolveTriggerComponent()
{
	Trigger = nullptr;

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TriggerCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (TriggerShape == EPuzzleTriggerShape::Box)
	{
		Trigger = TriggerBox;
		TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		Trigger = TriggerCapsule;
		TriggerCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void APuzzleReceiverBox::OnTriggerBeginOverlap(
	UPrimitiveComponent*, AActor* OtherActor,
	UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	if (!HasAuthority())
		return;

	if (bSolved)
		return;

	APuzzlePieceBase* Piece = Cast<APuzzlePieceBase>(OtherActor);
	if (!IsValid(Piece))
		return;

	// ExpectedId가 비어있으면(세팅 안됨) 처리 불가
	if (!ExpectedId.IsValid())
		return;

	if (Piece->PieceId.Id == ExpectedId.Id)
	{
		// 정답이면 조각 제거 + solved 처리
		Piece->Destroy();
		ServerMarkSolved();
	}
}

void APuzzleReceiverBox::ServerMarkSolved()
{
	if (!HasAuthority())
		return;

	if (bSolved)
		return;

	bSolved = true;

	// 서버에서도 이벤트 쏴주기
	OnSolved.Broadcast(this, true);
}

void APuzzleReceiverBox::OnRep_Solved()
{
	OnSolved.Broadcast(this, bSolved);
}

void APuzzleReceiverBox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APuzzleReceiverBox, ExpectedId);
	DOREPLIFETIME(APuzzleReceiverBox, bSolved);
}
