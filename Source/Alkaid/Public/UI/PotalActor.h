#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PotalActor.generated.h"

class UBoxComponent;
class UWidgetComponent;

UCLASS()
class ALKAID_API APotalActor : public AActor
{
	GENERATED_BODY()
	
public:	
	
	APotalActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PotalMesh")
	UStaticMeshComponent* PotalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BoxComponent")
	UBoxComponent* PotalCollision;	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UIComponent")
	TObjectPtr<UWidgetComponent> InteractionWidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TextComponent")
	TObjectPtr<UWidgetComponent> InteractionTextWidgetComponent;


	UFUNCTION()
	void OnPotalOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPotalOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
