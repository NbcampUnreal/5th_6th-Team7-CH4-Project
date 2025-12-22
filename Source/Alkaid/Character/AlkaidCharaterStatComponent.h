// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AlkaidCharaterStatComponent.generated.h"


DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, float/*Current*/, float/*Max*/);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ALKAID_API UAlkaidCharaterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAlkaidCharaterStatComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FOnStaminaChanged OnStaminaChanged;

	// Stamina
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Stamina, Category = "AlkaidCharacter|Stamina")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "AlkaidCharacter|Stamina")
	float MaxStamina;

	void AddStamina(float Amount); //server

	FORCEINLINE float GetStamina() const { return Stamina; }

	void SetStamina(float NewStamina); //server

	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }

	void SetMaxStamina(float NewMaxStamina); //server
	
	void AddMaxStamina(float Amount); //server
	
	void StaminaUsing(float DeltaTime, float Damage);//server

	void ApplyStamina();

	//speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Speed, Category = "AlkaidCharacter|Speed")
	float NomalSpeed;

	FORCEINLINE float GetNomalSpeed() const { return NomalSpeed; }

	void SetNomalSpeed(float NewNomalSpeed); //server
	
	void AddNomalSpeed(float Amount);//server

	void ApplySpeed();
	                                                                         

	//candle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "AlkaidCharacter|Candle")
	float CandleCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AlkaidCharacter|Candle")
	float MaxCandleCount = 3.0f;

	FORCEINLINE float GetCandleCount() const { return CandleCount; }

	void AddCandleCount(int32 Amount);//server

	void SetCandleCount(float NewCandleCount);//server

	//candle cooltime
	UPROPERTY(ReplicatedUsing=OnRep_CandleCoolDonwEndTime)
	float CandleCoolDownEndTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AlkaidCharacter|Candle")
	float CandleCooldownSeconds;

	bool IsCandleOnCooldown() const;

	UFUNCTION(BlueprintPure, Category = "AlkaidCharater|Candle")
	float GetCandleCooldownRemainingTime() const;

	void StartCandleCooldown();//server
	
	//sprint
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "AlkaidCharacter|Speed")
	float BaseWalkSpeed = 500;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "AlkaidCharacter|Speed")
	float SpeedBonus = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AlkaidCharacter|SprintSpeed")
	float SprintMultiplier = 1.5f;

	FORCEINLINE float GetSprintSpeed() const { return BaseWalkSpeed * SprintMultiplier + SpeedBonus; }

	FORCEINLINE float GetFinalMoveSpeed(bool bSprinting) const
	{
		const float Base = GetSprintSpeed();
		return bSprinting ? Base * SprintMultiplier : Base;
	}


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//OnRep Functions
	UFUNCTION()
	void OnRep_Stamina();
	UFUNCTION()
	void OnRep_Speed();	
	UFUNCTION()
	void OnRep_CandleCoolDonwEndTime();
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FTimerHandle StaminaTimerHandle;
};
