#include "Server/AlkaidGameModeBase.h"
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
	//로비 복귀 전 Ready 상태 초기화.
	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (AMyPlayerState* MyPS = Cast<AMyPlayerState>(PS))
		{
			MyPS->bIsReady = false;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Return to Lobby"));
	TravelTo(LobbyMapPath);
}

void AAlkaidGameModeBase::RetryGame()
{
	if (!HasAuthority())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Game Restart"));
	TravelTo(PuzzleMapPath);
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
