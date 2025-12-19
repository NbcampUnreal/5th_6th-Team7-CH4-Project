// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AlkiadAI.h"
#include "AI/AlkaidAIController.h"

// Sets default values
AAlkiadAI::AAlkiadAI()
{
	AIControllerClass = AAlkaidAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}


void AAlkiadAI::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("AI Pawn has been spawned."));
	
}

// Called every frame
void AAlkiadAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAlkiadAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

