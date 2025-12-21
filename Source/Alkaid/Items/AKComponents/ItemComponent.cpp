// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemComponent.h"
#include "Character/AlkaidCharacter.h"
#include "Controller/AlkaidPlayerController.h"
#include "Net/UnrealNetwork.h"

UItemComponent::UItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	
}

void UItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UItemComponent, Candle);

}

bool UItemComponent::PickupCandle(int32 CandleAmount)
{
	// 1단계 : 현재 Ammo가 MaxCapacity 이상인지 확인
	if (Candle >= CandleMaxCapacity)
	{
		UE_LOG(LogTemp, Warning, TEXT(" [ PickupCandle ] 촛불이 가득찼습니다! 현재 : %d / 최대 : %d "), Candle, CandleMaxCapacity);
		return false;	// 습득 실패
	}
	// 2단계 : 탄약 추가 (MaxCapacity 범위 내)
	Candle = FMath::Clamp(Candle + CandleAmount, 0, CandleMaxCapacity);
	SetCallHUDCandle();
	UE_LOG(LogTemp, Log, TEXT(" 남은 Ammo : %d "), Candle);
	return true;
}

void UItemComponent::SetCallHUDCandle()
{
	AKCharacter = AKCharacter == nullptr ? Cast<AAlkaidCharacter>(GetOwner()) : AKCharacter;
	if (AKCharacter)
	{
		AKController = AKController == nullptr ? Cast<AAlkaidPlayerController>(AKCharacter->Controller) : AKController;
		if (AKController)
		{
			AKController->SetHUDCandle(Candle);
			UE_LOG(LogTemp, Warning, TEXT(" SetCallHUDAmmo : HUD 업데이트 / 총알 : %d"), Candle);
		}
	}
}

void UItemComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
}

void UItemComponent::OnRep_Candle()
{
	SetCallHUDCandle();
}

void UItemComponent::SpendRound()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Candle = FMath::Clamp(Candle - 1, 0, CandleMaxCapacity);
		SetCallHUDCandle();
	}
	UE_LOG(LogTemp, Warning, TEXT(" SpendRound : 남은 총알 / %d"), Candle);
}


void UItemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

