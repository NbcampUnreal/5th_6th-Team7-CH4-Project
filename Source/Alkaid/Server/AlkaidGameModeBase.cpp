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
	Super::Logout(Exiting);

	if (!bReloadLevelWhenEmpty)
	{
		return;
	}

	if (bReloadIssued)
	{
		return;
	}

	if (GetNumPlayers() == 0)
	{
		bReloadIssued = true;

		ResetRoomOnReturn();

		GetWorld()->ServerTravel(ReloadLevelPath);
	}
}
