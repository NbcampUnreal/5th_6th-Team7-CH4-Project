#include "Gamemode/PuzzleColorButton.h"

#include "Net/UnrealNetwork.h"
#include "Components/ShapeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h" 

APuzzleColorButton::APuzzleColorButton()
{
	// 틱 체크로 안함 (메모리 사용줄일 목적)
	PrimaryActorTick.bCanEverTick = false;

	// 멀티에서 네트워크로 복제
	bReplicates = true;

	// 루트 컴포넌트
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// 외형 메시
	Visual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual"));
	Visual->SetupAttachment(Root);

	// 오버랩 이벤트가 불필요
	Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 트리거 박스
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox")); // 수정/추가: Box 전용 컴포넌트로 분리
	TriggerBox->SetupAttachment(Root);

	// 트리거 캡슐
	TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("TriggerCapsule")); // 수정/추가: Capsule 컴포넌트 추가
	TriggerCapsule->SetupAttachment(Root);

	// 트리거 크기, 블루프린트에서 조절가능
	TriggerBox->SetBoxExtent(FVector(60.f, 60.f, 30.f));
	TriggerCapsule->SetCapsuleSize(60.f, 30.f);

	// 트리거는 오버랩만 쓰므로 QueryOnly
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// 기본으로 모든 충돌 반응을 무시(캐릭터에게 걷어차야 날아갈이유없음)
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);

	// Pawn과 PhysicsBody는 오버랩
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	TriggerCapsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerCapsule->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);

	// 오버랩 이벤트
	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerCapsule->SetGenerateOverlapEvents(true);

	// 기본 선택 트리거 지정
	ResolveTriggerComponent(); // 선택된 트리거를 Trigger로 지정

	// 오버랩 이벤트 바인딩
	if (Trigger) //nullptr 방어
	{
		Trigger->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnTriggerBeginOverlap); // 수정/추가
		Trigger->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnTriggerEndOverlap);     // 수정/추가
	}
}

void APuzzleColorButton::BeginPlay()
{
	Super::BeginPlay();

	// 트리거 이벤트 바인딩
	// 서버에서만 눌림 판정

	// 실행 중에도 BP에서 TriggerShape를 바꿨을 수 있으므로 다시 선택
	ResolveTriggerComponent(); // 수정/추가: BeginPlay에서 재선택

	// 트리거가 없으면 버튼이 동작할 수 없으니 로그로 알려주고 종료
	if (!Trigger) // 수정/추가: TriggerBox/Capsule 둘 다 없을 때
	{
		UE_LOG(LogTemp, Warning, TEXT("PuzzleColorButton: Trigger(Box/Capsule) component not found."));
		return;
	}

	// 오버랩 이벤트 바인딩이 중복될 수 있으므로 제거 후 다시 바인딩
	Trigger->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnTriggerBeginOverlap);
	Trigger->OnComponentEndOverlap.RemoveDynamic(this, &ThisClass::OnTriggerEndOverlap);
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnTriggerBeginOverlap);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnTriggerEndOverlap);
}

void APuzzleColorButton::ResolveTriggerComponent()
{
	// 기본은 둘 다 비활성으로 시작
	Trigger = nullptr;

	if (TriggerBox)
	{
		TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (TriggerCapsule)
	{
		TriggerCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 선택된 모양에 따라 활성 트리거 결정
	if (TriggerShape == EPuzzleTriggerShape::Box)
	{
		if (TriggerBox)
		{
			Trigger = TriggerBox;
			TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}
	else
	{
		if (TriggerCapsule)
		{
			Trigger = TriggerCapsule;
			TriggerCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}

	// 둘 다 없거나 선택된 것이 없으면, 있는 쪽을 폴백으로 사용
	if (!Trigger)
	{
		if (TriggerBox)
		{
			Trigger = TriggerBox;
			TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		else if (TriggerCapsule)
		{
			Trigger = TriggerCapsule;
			TriggerCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}
}

void APuzzleColorButton::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// bPressed 값을 네트워크로 복제
	DOREPLIFETIME(APuzzleColorButton, bPressed);
}

void APuzzleColorButton::OnRep_Pressed()
{
	// 클라이언트에서 눌림 상태가 바뀌면 호출
	// 로직 판단은 서버에서 게임모드가
	BP_OnPressedVisualChanged(bPressed);
}

bool APuzzleColorButton::IsValidActivator(AActor* OtherActor) const // 수정/추가: 클래스명 일치
{
	// 유효확인
	if (!IsValid(OtherActor) || OtherActor == this)
	{
		return false;
	}

	// 태그 필터가 없으면 태그 검사x
	if (RequiredActorTag != NAME_None && !OtherActor->ActorHasTag(RequiredActorTag))
	{
		return false;
	}

	// 문제없으면 눌림 인정
	return true;
}

void APuzzleColorButton::OnTriggerBeginOverlap(
	UPrimitiveComponent*, AActor* OtherActor,
	UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	// 눌림 판정은 서버에서
	if (!HasAuthority())
	{
		return;
	}

	// 눌림 인정 대상이 아니면 무시
	if (!IsValidActivator(OtherActor))
	{
		return;
	}

	// 트리거 안에 들어온 유효 대상 수를 올림
	OverlapCount++;

	// 첫 번째로 들어온 순간에만 눌림
	if (OverlapCount == 1)
	{
		ServerSetPressed(true);
	}
}

void APuzzleColorButton::OnTriggerEndOverlap(
	UPrimitiveComponent*, AActor* OtherActor,
	UPrimitiveComponent*, int32)
{
	// 눌림 판정은 서버가
	if (!HasAuthority())
	{
		return;
	}

	// 눌림 인정 대상이 아니면 무시
	if (!IsValidActivator(OtherActor))
	{
		return;
	}

	// 트리거에서 나간 유효 대상 수를 내림
	OverlapCount = FMath::Max(0, OverlapCount - 1);

	// 모두없으면 해제
	if (OverlapCount == 0)
	{
		ServerSetPressed(false);
	}
}

void APuzzleColorButton::ServerSetPressed(bool bNewPressed)
{
	// 서버만 상태 변경 가능
	if (!HasAuthority())
	{
		return;
	}

	// 상태가 같으면 유지
	if (bPressed == bNewPressed)
	{
		return;
	}

	// 눌림 상태를 변경
	bPressed = bNewPressed;

	// 서버에서도 연출시 호출
	BP_OnPressedVisualChanged(bPressed);

	// 게임모드는 이 이벤트를 받아 조건을 계산
	OnPressedChanged.Broadcast(this, bPressed);
}