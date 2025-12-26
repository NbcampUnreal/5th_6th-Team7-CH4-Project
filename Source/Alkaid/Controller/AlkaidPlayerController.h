// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AlkaidPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ALKAID_API AAlkaidPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void SetHUDStamina(float Health, float MaxHealth);

	void SetHUDCandle(int32 Candle);



protected:
	virtual void BeginPlay() override;



private:
	class AAKHUD* AKHUD;
};
