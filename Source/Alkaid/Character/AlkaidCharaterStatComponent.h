// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AlkaidCharaterStatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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

	FORCEINLINE void AddStamina(float Amount) { Stamina = FMath::Clamp(Stamina + Amount, 0.0f, MaxStamina); }

	FORCEINLINE void AddStaminaDelta(float DeltaTime, float Rate) { AddStamina(DeltaTime * Rate); }

	FORCEINLINE float GetStamina() const { return Stamina; }

	FORCEINLINE void SetStamina(float NewStamina) { Stamina = FMath::Clamp(NewStamina, 0.0f, MaxStamina); }

	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }

	FORCEINLINE void SetMaxStamina(float NewMaxStamina) 
	{
		MaxStamina = FMath::Max(0.0f, NewMaxStamina);
		Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);
	}

	FORCEINLINE void AddMaxStamina(float Amount) 
	{ 
		SetMaxStamina(MaxStamina + Amount); 
	}	

	void StaminaUsing(float DeltaTime, float Damage);

	//speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Speed, Category = "AlkaidCharacter|Speed")
	float NomalSpeed;

	FORCEINLINE float GetNomalSpeed() const { return NomalSpeed; }

	FORCEINLINE void SetNomalSpeed(float NewNomalSpeed) 
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

	FORCEINLINE void AddNomalSpeed(float Amount) { SetNomalSpeed(NomalSpeed + Amount); }

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
