// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AlkaidCharaterStatComponent.h"

// Sets default values for this component's properties
UAlkaidCharaterStatComponent::UAlkaidCharaterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	MaxStamina = 100.0f;
	Stamina = MaxStamina;
	NomalSpeed = 500.0f;
}

void UAlkaidCharaterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{

}


void UAlkaidCharaterStatComponent::StaminaUsing(float DeltaTime, float Damage)
{
	Stamina = FMath::Clamp(Stamina - Damage * DeltaTime, 0.0f, MaxStamina);
}

// Called when the game starts
void UAlkaidCharaterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	SetNomalSpeed(NomalSpeed);
	
}

void UAlkaidCharaterStatComponent::OnRep_Stamina()
{

}

void UAlkaidCharaterStatComponent::OnRep_Speed()
{

}


// Called every frame
void UAlkaidCharaterStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

