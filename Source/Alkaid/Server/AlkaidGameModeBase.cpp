#include "Server/AlkaidGameModeBase.h"
#include "Server/AlkaidGameStateBase.h"
#include "MyPlayerState.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerController.h"

void AAlkaidGameModeBase::ReturnToLobby()
{
	if (!HasAuthority())
	{
		return;
	}

	ResetRoomOnReturn();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			PC->ClientTravel(LobbyMapPath, ETravelType::TRAVEL_Absolute);
		}
	}
}

void AAlkaidGameModeBase::ResetRoomOnReturn()
{
	if (!HasAuthority())
	{
		return;
	}

	if (GameState)
	{
		for (APlayerState* PS : GameState->PlayerArray)
		{
			if (AMyPlayerState* MyPS = Cast<AMyPlayerState>(PS))
			{
				MyPS->bInRoom = false;
				MyPS->bReady = false;
			}
		}
	}

	if (AAlkaidGameStateBase* GS = GetGameState<AAlkaidGameStateBase>())
	{
		GS->RoomState = ERoomState::Free;
		GS->bStartReady = false;
		GS->RoomPlayerCount = 0;
		GS->RoomReadyCount = 0;
	}
}

void AAlkaidGameModeBase::Logout(AController* Exiting)
{
	UE_LOG(LogTemp, Warning, TEXT("[Logout] ENTER Reload=%d Issued=%d Num(before)=%d"),
		bReloadLevelWhenEmpty ? 1 : 0, bReloadIssued ? 1 : 0, GetNumPlayers());

	Super::Logout(Exiting);

	UE_LOG(LogTemp, Warning, TEXT("[Logout] AFTER Super Num=%d"), GetNumPlayers());

	if (!bReloadLevelWhenEmpty || bReloadIssued)
		return;

	if (GetNumPlayers() == 0)
	{
		bReloadIssued = true;
		UE_LOG(LogTemp, Warning, TEXT("[Logout] ServerTravel -> %s"), *ReloadLevelPath);
		GetWorld()->ServerTravel(ReloadLevelPath);
	}

	GetWorldTimerManager().ClearTimer(ReloadCheckTimer);
	GetWorldTimerManager().SetTimer(ReloadCheckTimer, this, &AAlkaidGameModeBase::TryReloadIfEmpty, 0.2f, false);
}

void AAlkaidGameModeBase::TryReloadIfEmpty()
{
	if (!bReloadLevelWhenEmpty || bReloadIssued)
	{
		return;
	}

	const int32 Players = GetNumPlayers();
	const int32 Specs = GetNumSpectators();
	const int32 PlayerArrayNum = GameState ? GameState->PlayerArray.Num() : -1;

	UE_LOG(LogTemp, Warning, TEXT("[ReloadCheck] Players=%d Specs=%d PlayerArray=%d Issued=%d"),
		Players, Specs, PlayerArrayNum, bReloadIssued ? 1 : 0);

	if ((Players + Specs) == 0)
	{
		bReloadIssued = true;
		GetWorld()->ServerTravel(ReloadLevelPath);
	}
}
