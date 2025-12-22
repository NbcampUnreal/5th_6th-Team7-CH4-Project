// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AlkiadAICharacter.generated.h"

UCLASS()
class ALKAID_API AAlkiadAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	
	AAlkiadAICharacter();

protected:
	
	virtual void BeginPlay() override;

public:	
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
