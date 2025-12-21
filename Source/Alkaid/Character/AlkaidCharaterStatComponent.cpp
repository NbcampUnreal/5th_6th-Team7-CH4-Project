// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AlkaidCharaterStatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Alkaid/Controller/AlkaidPlayerController.h"
#include "Alkaid/Character/AlkaidCharacter.h"

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
	CandleCooldownSeconds = 3.0f;
}

void UAlkaidCharaterStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);	

	DOREPLIFETIME(UAlkaidCharaterStatComponent, Stamina);
	DOREPLIFETIME(UAlkaidCharaterStatComponent, MaxStamina);
	DOREPLIFETIME(UAlkaidCharaterStatComponent, NomalSpeed);
	DOREPLIFETIME(UAlkaidCharaterStatComponent, CandleCount);
	DOREPLIFETIME(UAlkaidCharaterStatComponent, CandleCoolDownEndTime);
}

void UAlkaidCharaterStatComponent::AddStamina(float Amount)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Stamina = FMath::Clamp(Stamina + Amount, 0.0f, MaxStamina);
	}
}

void UAlkaidCharaterStatComponent::SetStamina(float NewStamina)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Stamina = FMath::Clamp(NewStamina, 0.0f, MaxStamina);
	}
}

void UAlkaidCharaterStatComponent::SetMaxStamina(float NewMaxStamina)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		MaxStamina = FMath::Max(0.0f, NewMaxStamina);
		Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);
	}
}

void UAlkaidCharaterStatComponent::AddMaxStamina(float Amount)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		MaxStamina = FMath::Max(0.0f, MaxStamina + Amount);
	}
}


void UAlkaidCharaterStatComponent::StaminaUsing(float DeltaTime, float Damage)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
		return;
	Stamina = FMath::Clamp(Stamina - Damage * DeltaTime, 0.0f, MaxStamina);
}

void UAlkaidCharaterStatComponent::ApplyStamina()
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
		if(!PawnOwner)
			return;
		if(!PawnOwner->IsLocallyControlled())
			return;	
	
		OnStaminaChanged.Broadcast(Stamina, MaxStamina);
}

void UAlkaidCharaterStatComponent::SetNomalSpeed(float NewNomalSpeed)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		NomalSpeed = NewNomalSpeed;
		ACharacter* Character = Cast<ACharacter>(GetOwner());
		if (Character)
		{
			Character->GetCharacterMovement()->MaxWalkSpeed = NomalSpeed;
		}
	}
}

void UAlkaidCharaterStatComponent::AddNomalSpeed(float Amount)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		NomalSpeed += Amount;
		ACharacter* Character = Cast<ACharacter>(GetOwner());
		if (Character)
		{
			Character->GetCharacterMovement()->MaxWalkSpeed = NomalSpeed;
		}
	}
}

void UAlkaidCharaterStatComponent::ApplySpeed()
{
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = NomalSpeed;
		}
	}
}

void UAlkaidCharaterStatComponent::AddCandleCount(float Amount)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		CandleCount =  FMath::Clamp(CandleCount + Amount, 0.0f, MaxCandleCount);
	}
}

void UAlkaidCharaterStatComponent::SetCandleCount(float NewCandleCount)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		CandleCount = FMath::Clamp(NewCandleCount, 0.0f, MaxCandleCount);
	}
}

bool UAlkaidCharaterStatComponent::IsCandleOnCooldown() const
{
	const UWorld* World = GetWorld();
	return World && World->GetTimeSeconds() < CandleCoolDownEndTime;
}

float UAlkaidCharaterStatComponent::GetCandleCooldownRemainingTime() const
{
	const UWorld* World = GetWorld();
	if(!World)
		return 0.0f;
	return FMath::Max(0.0f, CandleCoolDownEndTime - World->GetTimeSeconds());
}

void UAlkaidCharaterStatComponent::StartCandleCooldown()
{
	if(!GetOwner() || !GetOwner()->HasAuthority())
		return;
	CandleCoolDownEndTime = UGameplayStatics::GetTimeSeconds(GetWorld()) + CandleCooldownSeconds;
}



// Called when the game starts
void UAlkaidCharaterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Stamina = MaxStamina;
	}
	ApplyStamina();
	ApplySpeed();

	if (!AKCharacter)
	{
		AKCharacter = Cast<AAlkaidCharacter>(GetOwner());
	}
	AKPlayerController = Cast<AAlkaidPlayerController>(AKCharacter->Controller);
	if (AKPlayerController)
	{
		AKPlayerController->SetHUDStamina(Stamina, MaxStamina);
	}
}

void UAlkaidCharaterStatComponent::OnRep_Stamina()
{
	ApplyStamina();
}

void UAlkaidCharaterStatComponent::OnRep_Speed()
{
	ApplySpeed();
}

void UAlkaidCharaterStatComponent::OnRep_CandleCoolDonwEndTime()
{

}


// Called every frame
void UAlkaidCharaterStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

