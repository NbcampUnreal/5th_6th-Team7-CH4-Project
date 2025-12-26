// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AlkiadAICharacter.h"
#include "AI/AlkaidAIController.h"

// Sets default values
AAlkiadAICharacter::AAlkiadAICharacter()
{
	AIControllerClass = AAlkaidAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}


void AAlkiadAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


void AAlkiadAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

