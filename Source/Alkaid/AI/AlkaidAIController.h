// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AlkaidAIController.generated.h"

/**
 * 
 */
UCLASS()
class ALKAID_API AAlkaidAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AAlkaidAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;
};
