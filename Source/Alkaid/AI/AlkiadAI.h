// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AlkiadAI.generated.h"

UCLASS()
class ALKAID_API AAlkiadAI : public APawn
{
	GENERATED_BODY()

public:
	
	AAlkiadAI();

protected:
	
	virtual void BeginPlay() override;

public:	
	
	virtual void Tick(float DeltaTime) override;

	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
