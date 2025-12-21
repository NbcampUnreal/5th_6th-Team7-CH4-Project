// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/AlkaidPlayerController.h"
#include "Alkaid/HUD/AKHUD.h"
#include "Alkaid/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


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
	}
}