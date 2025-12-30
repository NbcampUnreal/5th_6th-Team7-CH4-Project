#include "UI/EndingActor.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/AlkaidCharacter.h"


AEndingActor::AEndingActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	EndingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EndingMesh"));
	EndingMesh->SetupAttachment(RootComponent);

	EndingBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("EndingBoxCollision"));
	EndingBoxCollision->SetupAttachment(RootComponent);

	EndingBoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AEndingActor::OnEndingOverlapBegin);
}


void AEndingActor::OnEndingOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}