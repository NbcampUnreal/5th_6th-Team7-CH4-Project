// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AlkaidCharaterStatComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ALKAID_API UAlkaidCharaterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAlkaidCharaterStatComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Stamina
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Stamina, Category = "AlkaidCharacter|Stamina")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "AlkaidCharacter|Stamina")
	float MaxStamina;

	void AddStamina(float Amount); //서버

	FORCEINLINE float GetStamina() const { return Stamina; }

	void SetStamina(float NewStamina); //서버

	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }

	void SetMaxStamina(float NewMaxStamina); //서버
	
	void AddMaxStamina(float Amount); //서버
	
	void StaminaUsing(float DeltaTime, float Damage);//서버

	//speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Speed, Category = "AlkaidCharacter|Speed")
	float NomalSpeed;

	FORCEINLINE float GetNomalSpeed() const { return NomalSpeed; }

	FORCEINLINE void SetNomalSpeed(float NewNomalSpeed) //서버
	{ 
		NomalSpeed = NewNomalSpeed; 
		if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
		{
			if(UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
			{
				MoveComp->MaxWalkSpeed = NomalSpeed;
			}
		}
	}

	FORCEINLINE void AddNomalSpeed(float Amount) { SetNomalSpeed(NomalSpeed + Amount); }//서버

	float CandleCount;

	FORCEINLINE void AddCandleCount(float Amount) { CandleCount += Amount; }//서버	


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_Stamina();
	UFUNCTION()
	void OnRep_Speed();	


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
