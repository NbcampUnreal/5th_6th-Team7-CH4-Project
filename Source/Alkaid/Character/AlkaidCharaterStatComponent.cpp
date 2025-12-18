// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AlkaidCharaterStatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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
	CandleCount = 0.0f;
}

void UAlkaidCharaterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);	

	DOREPLIFETIME(UAlkaidCharaterStatComponent, Stamina);
	DOREPLIFETIME(UAlkaidCharaterStatComponent, MaxStamina);
	DOREPLIFETIME(UAlkaidCharaterStatComponent, NomalSpeed);
}

void UAlkaidCharaterStatComponent::AddStamina(float Amount)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Stamina += Amount;
	}
}

void UAlkaidCharaterStatComponent::SetStamina(float NewStamina)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Stamina = NewStamina;
	}
}

void UAlkaidCharaterStatComponent::SetMaxStamina(float NewMaxStamina)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		MaxStamina = NewMaxStamina;
	}
}

void UAlkaidCharaterStatComponent::AddMaxStamina(float Amount)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		MaxStamina += Amount;
	}
}


void UAlkaidCharaterStatComponent::StaminaUsing(float DeltaTime, float Damage)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
		return;
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

