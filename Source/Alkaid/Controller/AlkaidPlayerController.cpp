// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/AlkaidPlayerController.h"
#include "Alkaid/HUD/AKHUD.h"
#include "Alkaid/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HUD/StaminaWidget_UI.h"


void AAlkaidPlayerController::BeginPlay()
{
	Super::BeginPlay();

	AKHUD = Cast<AAKHUD>(GetHUD());
}

void AAlkaidPlayerController::SetHUDStamina(float Stamina, float MaxStamina)
{
	AKHUD = AKHUD == nullptr ? Cast<AAKHUD>(GetHUD()) : AKHUD;
	bool bHUDValid = AKHUD &&
		AKHUD->CharacterOverlay &&
		AKHUD->CharacterOverlay->StaminaBar &&
		AKHUD->CharacterOverlay->StaminaText;
	if (bHUDValid)
	{
		const float StaminaPercent = Stamina / MaxStamina;
		AKHUD->CharacterOverlay->StaminaBar->SetPercent(StaminaPercent);
		FString StaminaText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Stamina), FMath::CeilToInt(MaxStamina));
		AKHUD->CharacterOverlay->StaminaText->SetText(FText::FromString(StaminaText));
		AKHUD->CharacterOverlay->StaminaWidget->UpdateStaminaBar(Stamina, MaxStamina); // 추가
	}
}

void AAlkaidPlayerController::SetHUDCandle(int32 Candle)
{
	AKHUD = AKHUD == nullptr ? Cast<AAKHUD>(GetHUD()) : AKHUD;

	bool bHUDValid = AKHUD &&
		AKHUD->CharacterOverlay &&
		AKHUD->CharacterOverlay->CandleAmount;

	if (bHUDValid)
	{
		FString CandleText = FString::Printf(TEXT( "%d" ), Candle);
		AKHUD->CharacterOverlay->CandleAmount->SetText(FText::FromString(CandleText));
	}

	UE_LOG(LogTemp, Warning, TEXT("SetHUDCandle() 함수 호출 양호 "));
}
