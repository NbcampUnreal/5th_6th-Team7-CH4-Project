#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Actor_UIessage.generated.h"

class UUserWidget;

UCLASS()
class ALKAID_API AActor_UIessage : public AActor
{
	GENERATED_BODY()
	
public:	

    AActor_UIessage();
	
	virtual void BeginPlay() override;


    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BoxComponent")
    class UBoxComponent* CollisionBox;

    UPROPERTY(EditAnywhere, Category = "MapsMessage")
    TSubclassOf<UUserWidget> MessageWidgetClass;

    UPROPERTY()
    TObjectPtr<UUserWidget> MessageWidgetInstance;

    UFUNCTION()
    void OnBoxOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnBoxOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    //트리거 한번 띄우고 더이상 안띄우기
	bool bHasMessageDisplayed = false;

};
