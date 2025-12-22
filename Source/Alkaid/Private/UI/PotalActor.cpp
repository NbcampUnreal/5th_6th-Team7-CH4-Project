#include "UI/PotalActor.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/AlkaidCharacter.h"
#include "Server/MyPlayerState.h"

APotalActor::APotalActor()
{

    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    PotalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PotalMesh"));
    PotalMesh->SetupAttachment(RootComponent);

    //상호작용 
    InteractionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidgetComponent"));
    InteractionWidgetComponent->SetupAttachment(RootComponent);
    InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
    InteractionWidgetComponent->SetDrawAtDesiredSize(true);
    InteractionWidgetComponent->SetVisibility(true);

    InteractionTextWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionText"));
    InteractionTextWidgetComponent->SetupAttachment(RootComponent);
    InteractionTextWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
    InteractionTextWidgetComponent->SetDrawAtDesiredSize(true);


    PotalCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("PotalCollision"));
    PotalCollision->SetupAttachment(RootComponent);

    PotalCollision->OnComponentBeginOverlap.AddDynamic(this, &APotalActor::OnPotalOverlapBegin);
    PotalCollision->OnComponentEndOverlap.AddDynamic(this, &APotalActor::OnPotalOverlapEnd);
}

void APotalActor::BeginPlay()
{
    Super::BeginPlay();

    if (InteractionWidgetComponent)
    {
        InteractionWidgetComponent->SetVisibility(false);
    }

    if (InteractionTextWidgetComponent)
    {
        InteractionTextWidgetComponent->SetVisibility(false);
    }
}

void APotalActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

	// 상호작용 위젯이 보이는 경우에만 카메라를 향하도록 회전
    if (InteractionWidgetComponent && InteractionWidgetComponent->IsVisible())
    {
        APlayerController* CameraRotation = GetWorld()->GetFirstPlayerController();
        if (CameraRotation && CameraRotation->PlayerCameraManager)
        {
            FVector CameraLocation = CameraRotation->PlayerCameraManager->GetCameraLocation();
            FVector WidgetLocation = InteractionWidgetComponent->GetComponentLocation();

            FRotator NewRotation = (CameraLocation - WidgetLocation).Rotation();

            InteractionWidgetComponent->SetWorldRotation(NewRotation);
        }
    }
}

void APotalActor::OnPotalOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (AAlkaidCharacter* OverlapInter = Cast<AAlkaidCharacter>(OtherActor))
    {
        if (OverlapInter->IsLocallyControlled() && InteractionWidgetComponent)
        {
            InteractionWidgetComponent->SetVisibility(true);
        }

        if (OverlapInter->IsLocallyControlled() && InteractionTextWidgetComponent)
        {
			InteractionTextWidgetComponent->SetVisibility(true);
        }
    }
}

void APotalActor::OnPotalOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (AAlkaidCharacter* OverlapInter = Cast<AAlkaidCharacter>(OtherActor))
    {
        if (OverlapInter->IsLocallyControlled() && InteractionWidgetComponent)
        {
            InteractionWidgetComponent->SetVisibility(false);
        }

        if (OverlapInter->IsLocallyControlled() && InteractionTextWidgetComponent)
        {
            InteractionTextWidgetComponent->SetVisibility(false);
        }
    }
}