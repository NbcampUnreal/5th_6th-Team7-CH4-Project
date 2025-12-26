// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AlkaidAIController.h"

AAlkaidAIController::AAlkaidAIController()
{
}

void AAlkaidAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if(InPawn) 
	{
		UE_LOG(LogTemp, Warning, TEXT("AI Controller is sontrolling."), *InPawn->GetName());
	}
}
