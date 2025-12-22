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



public:	
	

		
};
