// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALKAID_API UItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UItemComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	friend class AAlkaidCharacter;	// 전방선언 효과 

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool PickupCandle(int32 CandleAmount);
	
	// Candle 변경 호출 HUD
	void SetCallHUDCandle();
protected:
	
	virtual void BeginPlay() override;


private:
	UPROPERTY()
	class AAlkaidCharacter* AKCharacter;
	UPROPERTY()
	class AAlkaidPlayerController* AKController;

	/*
	* =============== Candle 관련 ===============
	*/
	// 추후 Candle로 변경 - 현재 사용 가능한 양
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Candle)
	int32 Candle;

	UPROPERTY(EditAnywhere)
	int32 CandleMaxCapacity = 3;

	// Candle 개수의 변화가 있을 때 Replicated 해줄 함수 호출용
	UFUNCTION()
	void OnRep_Candle();

	UFUNCTION()
	// Candle 1개 소비 / 해당 Candle의 유효한 소유자 확인 - 해당 소유자 HUD 업데이트
	void SpendRound();


public:	
	FORCEINLINE int32 GetCandle() const { return Candle; }
	FORCEINLINE void SetCandle(float Amount) { Candle = Amount; }
	FORCEINLINE int32 GetMaxCandle() const { return CandleMaxCapacity; }

		
};
