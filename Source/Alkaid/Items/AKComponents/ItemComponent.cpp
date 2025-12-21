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
		UE_LOG(LogTemp, Log, TEXT(" [ PickupCandle ] 촛불이 가득찼습니다! 현재 : %d / 최대 : %d "), Candle, CandleMaxCapacity);
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
	if (AKCharacter == nullptr) return;

	if (AKController == nullptr)
	{
		AKController = Cast<AAlkaidPlayerController>(AKCharacter->Controller);
	}

	if (AKController)
	{
		//AKController->SetHUDCandle(Candle);
		//UE_LOG(LogTemp, Warning, TEXT(" SetCallHUDAmmo : 발사 HUD 업데이트 / 총알 : %d"), Candle);
	}
	else
	{
		// 디버깅을 위해 컨트롤러를 찾지 못했음을 로그로 출력할 수 있습니다.
		UE_LOG(LogTemp, Warning, TEXT(" SetCallHUDAmmo : 컨트롤러를 찾을 수 없거나 형 변환 실패!"));
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


void UItemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

