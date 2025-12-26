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

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC)
		{
			continue;
		}

		PC->ClientTravel(LobbyMapPath, ETravelType::TRAVEL_Absolute);
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
