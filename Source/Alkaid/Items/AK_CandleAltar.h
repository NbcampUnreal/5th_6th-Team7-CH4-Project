// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AK_Pickup.h"
#include "AK_CandleAltar.generated.h"

/**
 * 범위 기반 지속 회복 아이템
 * - 플레이어가 범위에 들어가면 지속적으로 체력 회복
 * - 범위를 벗어나면 회복 중단
 * - 여러 플레이어를 동시에 회복 가능
 * - 시각적 피드백 (범위 내 플레이어 감지 시 강조)
 * - 아이템은 파괴되지 않음 (영구적인 회복 지역)
 */
UCLASS()
class ALKAID_API AAK_CandleAltar : public AAK_Pickup
{
	GENERATED_BODY()
	
public:
	AAK_CandleAltar();

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;


protected:
	// 범위에 진입했을 때 처리
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;

	// 범위에서 벗어났을 때 처리 (새로 추가)
	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	virtual void BeginPlay() override;

	// 범위 내 플레이어 감지 상태에 따른 시각적 피드백
	void UpdateVisualFeedback();

	// 플레이어의 죽음 감지
	UFUNCTION()
	void OnCharacterElimmed(AActor* ElimedCharacter);

	/*
	// ===================== 시각적 효과 관련 RPC 함수 추가 =====================
	*/
	// 모든 클라이언트에 픽업 이펙트 재생 명령 (멀티캐스트)
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayPickupEffect();

	// 모든 클라이언트에 시각 피드백 업데이트 명령
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastUpdateVisualFeedback(bool bPlayerInRange);

private:
	// 범위 기반 회복 아이템 설정
	UPROPERTY(EditAnywhere, Category = "Health", meta = (ClampMin = "0.1", ClampMax = "1000"))
	float HealAmount = 50.f;

	// 한 번에 회복에 걸리는 시간
	UPROPERTY(EditAnywhere, Category = "Health", meta = (ClampMin = "0.1", ClampMax = "60"))
	float HealingTime = 5.f;

	// 범위 내에 계속 있을 때 재발동 딜레이 (시간 경과 후 자동 재발동)
	UPROPERTY(EditAnywhere, Category = "Health", meta = (ClampMin = "0.1", ClampMax = "60"))
	float ReHealDelay = 2.f;

	// 시각적 피드백 설정
	UPROPERTY(EditAnywhere, Category = "Visual")
	float HighlightIntensity = 2.0f; // 강조 효과 강도

	UPROPERTY(EditAnywhere, Category = "Visual")
	bool bEnableVisualFeedback = true; // 시각적 피드백 활성화 여부

	// 현재 범위 내에 있는 모든 캐릭터들
	UPROPERTY()
	TArray<class AAlkaidCharacter*> CharactersInRange;

	// 재활성화 타이머 (각 플레이어별)
	TMap<class AAlkaidCharacter*, FTimerHandle> ReHealTimerHandles;

	// 자동 복제되는 회전값 
	UPROPERTY(Replicated)
	FRotator ReplicatedRotation = FRotator::ZeroRotator;

	// 범위내 플레이어가 있을시 시각적 피드백 활성여부 컨트롤
	UPROPERTY(ReplicatedUsing = OnRep_HasCharactersInRange)
	bool bHasCharactersInRange = false;

	UFUNCTION()
	void OnRep_HasCharactersInRange();


	UFUNCTION()
	// 타이머 콜백 함수
	void ReactivateHealing(AAlkaidCharacter* Character);

	UFUNCTION()
	// 범위 내 플레이어 확인
	bool IsCharacterInRange(AAlkaidCharacter* Character) const;

	UFUNCTION()
	// 범위 내 모든 플레이어 회복 상태 갱신
	void UpdateAllCharactersInRange();

};
