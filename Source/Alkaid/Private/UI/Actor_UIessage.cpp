#include "UI/Actor_UIessage.h"
#include "Blueprint/UserWidget.h"

AActor_UIessage::AActor_UIessage()
{
	// 루트
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	// 박스 생성
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);
	// 이벤트
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AActor_UIessage::OnBoxOverlapBegin);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &AActor_UIessage::OnBoxOverlapEnd);
}

void AActor_UIessage::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(MessageWidgetClass) == true)
	{
		MessageWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), MessageWidgetClass);
		
	}
}

void AActor_UIessage::OnBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(MessageWidgetInstance) == true)
	{
		MessageWidgetInstance->AddToViewport();
	}
}

void AActor_UIessage::OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsValid(MessageWidgetInstance) == true)
	{
		MessageWidgetInstance->RemoveFromViewport();
	}
}
// IsLocalPlayerController 해서 로컬 플레이어인지 확인해서 플레이어일때만 메세지 띄우기 해야함
// 캐릭터 받으면