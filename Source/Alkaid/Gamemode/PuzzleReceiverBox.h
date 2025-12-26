#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gamemode/PuzzleTypes.h"
#include "PuzzleReceiverBox.generated.h"

class UShapeComponent;
class UBoxComponent;
class UCapsuleComponent;

class APuzzlePieceBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReceiverSolved, class APuzzleReceiverBox*, Receiver, bool, bSolved);

UCLASS()
class ALKAID_API APuzzleReceiverBox : public AActor
{
	GENERATED_BODY()

public:
	APuzzleReceiverBox();

	// 레벨 인스턴스마다 고정으로 부여(수집용)
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Puzzle|Receiver")
	FPuzzleId ReceiverId;

	// 이번 판의 정답 ID(게임 중 GameMode가 세팅)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Puzzle|Receiver")
	FPuzzleId ExpectedId;

	UPROPERTY(ReplicatedUsing = OnRep_Solved, BlueprintReadOnly, Category = "Puzzle|Receiver")
	bool bSolved = false;

	UPROPERTY(BlueprintAssignable, Category = "Puzzle|Receiver")
	FOnReceiverSolved OnSolved;

	// 트리거 모양 선택(버튼이랑 동일)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puzzle")
	EPuzzleTriggerShape TriggerShape = EPuzzleTriggerShape::Box;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_Solved();

	UFUNCTION()
	void OnTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	void ResolveTriggerComponent();
	void ServerMarkSolved();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerBox = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UCapsuleComponent> TriggerCapsule = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UShapeComponent> Trigger = nullptr;
};
