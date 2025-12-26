#include "Server/AlkaidGameModeBase.h"
#include "Server/AlkaidGameStateBase.h"
#include "MyPlayerState.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"

void AAlkaidGameModeBase::ReturnToLobby()
{
	if (!HasAuthority())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Return to Lobby"));
	ResetRoomOnReturn();
	TravelTo(LobbyMapPath);
}

void AAlkaidGameModeBase::TravelTo(const FString& MapPath)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (MapPath.IsEmpty())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("TravelTo : %s"), *MapPath);
	World->ServerTravel(*MapPath);
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
				MyPS->bActorSeamlessTraveled = false;
				MyPS->bActorSeamlessTraveled = false;
			}
		}
	}

	if (AAlkaidGameStateBase* GS = GetGameState<AAlkaidGameStateBase>())
	{
		GS->RoomState = ERoomState::Free;
		GS->RoomLeaderPS = nullptr;
		GS->bStartReady = false;
		GS->RoomPlayerCount = 0;
		GS->RoomReadyCount = 0;
	}
}
