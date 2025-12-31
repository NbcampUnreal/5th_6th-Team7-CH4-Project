#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gamemode/PuzzleTypes.h"
#include "PuzzleBlockableDoor.generated.h"

class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class ALKAID_API APuzzleBlockableDoor : public AActor
{
	GENERATED_BODY()

public:
	APuzzleBlockableDoor();

	// 예: 2_3_YellowDoor
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Puzzle|Door")
	FPuzzleId DoorId;

	// 문 제어: 버튼이 눌린 동안만 열림 유지
	UFUNCTION(BlueprintCallable, Category = "Puzzle|Door")
	void SetActive(bool bInActive);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> Root = nullptr;

	// 왼쪽/오른쪽 패널 (서로 반대 방향으로 이동)
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PanelL = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PanelR = nullptr;

	// 완전히 열렸을 때, 두 패널 사이 간격(Offset)
	UPROPERTY(EditAnywhere, Category = "Puzzle|Door")
	float OpenDistance = 300.f;

	UPROPERTY(EditAnywhere, Category = "Puzzle|Door")
	float MoveSpeed = 800.f;

	// 0=닫힘, OpenDistance=완전 열림 (RepNotify로 클라 동기화)
	UPROPERTY(ReplicatedUsing = OnRep_CurrentOffset)
	float CurrentOffset = 0.f;

	// 버튼 입력 상태(서버 권한)
	UPROPERTY(Replicated)
	bool bActive = false;

	UFUNCTION()
	void OnRep_CurrentOffset();

	// 서버에서만 호출: 한 틱 이동
	void ServerStepMove(float DeltaSeconds);

	// 오프셋 값으로 패널 위치 세팅(클라/서버 공용)
	void ApplyOffset(float InOffset);

	// 현재 패널 상대좌표로부터 오프셋 재계산
	float CalcOffsetFromPanels() const;

private:
	// 닫힘(기준) 상대 위치 캐시 (BP에서 배치한 "닫힘 상태"를 기준으로 삼기 위함)
	UPROPERTY() FVector ClosedRelL;
	UPROPERTY() FVector ClosedRelR;
	UPROPERTY() bool bClosedRelCached = false;

	void CacheClosedRelativeIfNeeded();
};