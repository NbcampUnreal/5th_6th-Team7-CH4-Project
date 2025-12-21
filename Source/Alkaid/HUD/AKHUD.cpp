// Fill out your copyright notice in the Description page of Project Settings.


#include "AKHUD.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"


void AAKHUD::BeginPlay()
{
	Super::BeginPlay();

	AddCharacterOverlay();
}

void AAKHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}


void AAKHUD::DrawHUD()
{
	Super::DrawHUD();

	// 추후 낚시대, 몽둥이 관련 조준 UI 로직 작성 
}