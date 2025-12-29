#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndingActor.generated.h"

class UBoxComponent;
class UWidgetComponent;

UCLASS()
class ALKAID_API AEndingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AEndingActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EndingMesh")
	UStaticMeshComponent* EndingMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EndingBoxComponent")
	UBoxComponent* EndingBoxCollision;

	UFUNCTION()
	void OnEndingOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
