#include "Server/LobbyGameModeBase.h"
#include "MyPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

ALobbyGameModeBase::ALobbyGameModeBase()
{
	MinPlayerCount = 2;
	//로비에서 사용되는 PlayerState.
	PlayerStateClass = AMyPlayerState::StaticClass();
}

void ALobbyGameModeBase::CheckAllReady()
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

	int32 PlayerCount = 0;
	int32 ReadyCount = 0;

	//현재 접속한 플레이어들의 Ready 상태 확인.
	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC)
		{
			continue;
		}

		if (AMyPlayerState* PS = PC->GetPlayerState<AMyPlayerState>())
		{
			PlayerCount++;
			if (PS->bIsReady)
			{
				ReadyCount++;
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("CheckAllReady Player = %d Ready = %d Min = %d"), PlayerCount, ReadyCount, MinPlayerCount);

	//최소 인원 + 전원 Ready 상태이면 StartGame 호출.
	if (PlayerCount >= MinPlayerCount && ReadyCount == PlayerCount)
	{
		StartGame();
	}
}

void ALobbyGameModeBase::StartGame()
{
	if (!HasAuthority())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("StartGame All Ready -> TravelTo PuzzleMap"));
	
	bUseSeamlessTravel = true;
	TravelTo(PuzzleMapPath);
}
