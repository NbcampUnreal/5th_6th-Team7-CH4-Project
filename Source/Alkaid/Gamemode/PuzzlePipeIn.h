#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gamemode/PuzzleTypes.h"
#include "PuzzlePipeIn.generated.h"

class UStaticMeshComponent;
class UShapeComponent;
class UBoxComponent;
class UCapsuleComponent;
class UPrimitiveComponent;
class USceneComponent;

class APuzzlePipeOut;
class APuzzlePieceBase;

UCLASS()
class ALKAID_API APuzzlePipeIn : public AActor
{
	GENERATED_BODY()

public:
	APuzzlePipeIn();

	// 어디로 보낼지(레벨에서 인스턴스별로 지정)
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Puzzle|Pipe")
	TObjectPtr<APuzzlePipeOut> LinkedOutPipe = nullptr;

	// 트리거 형태 Box / Capsule 중 선택
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puzzle|Pipe")
	EPuzzleTriggerShape TriggerShape = EPuzzleTriggerShape::Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

	// 공용 트리거 포인터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UShapeComponent> Trigger = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerBox = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCapsuleComponent> TriggerCapsule = nullptr;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	void ResolveTriggerComponent();
	void ApplyTriggerCollisionDefaults(UShapeComponent* InTrigger);
};