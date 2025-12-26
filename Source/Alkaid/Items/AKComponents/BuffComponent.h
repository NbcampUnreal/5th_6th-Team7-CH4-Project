// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALKAID_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UBuffComponent();

	friend class AAlkaidCharacter;	// 전방선언 효과 

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 회복 시작
	void Heal(float HealAmount, float HealingTime);

	// 회복 중단 (범위 기반 아이템용)
	void StopHealing();

	// 속도 버프
	void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime);
	// 초기 속도 설정 함수 - 캐릭터에서 호출하기 위해
	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);

protected:
	
	virtual void BeginPlay() override;

	// = 회복 = 매 프레임마다 호출해서 치유
	void HealRampUp(float DeltaTime);

private:
	UPROPERTY()
	class AAlkaidCharacter* AKCharacter;
	UPROPERTY()
	class AAlkaidPlayerController* AKController;
	UPROPERTY()
	class UAlkaidCharaterStatComponent* AKStatComp;


	/*
	===================== Heal Buff =====================
	*/
	bool bHealing = false;
	float HealingRate = 0;
	float AmountToHeal = 0.f;

	/*
	=====================	Speed buff	=====================
	*/
	FTimerHandle SpeedBuffTiemr;
	
	// 초기 스피드
	float InitialBaseSpeed;
	float InitialCrouchSpeed;
	
	void ResetSpeeds();

	// 서버와 클라의 스피드 차이를 없애기위한 RPC 함수
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float BaseSpeed, float CrouchSpeed);

public:	
	

		
};
