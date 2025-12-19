#include "UI/Actor_UIessage.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "Character/AlkaidCharacter.h"

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

	if (bHasMessageDisplayed) return;

	AAlkaidCharacter* OverlapCharacter = Cast<AAlkaidCharacter>(OtherActor);

	if (OverlapCharacter && OverlapCharacter->IsLocallyControlled())
	{
		if (IsValid(MessageWidgetClass) == true)
		{
			if (IsValid(MessageWidgetInstance))
			{
				MessageWidgetInstance->AddToViewport();

				bHasMessageDisplayed = true;
			}
		}
	}
}

void AActor_UIessage::OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AAlkaidCharacter* OverlapCharacter = Cast<AAlkaidCharacter>(OtherActor);

	if (OverlapCharacter && OverlapCharacter->IsLocallyControlled())
	{
		if (IsValid(MessageWidgetInstance) == true)
		{
			MessageWidgetInstance->RemoveFromViewport();
		}
	}
}