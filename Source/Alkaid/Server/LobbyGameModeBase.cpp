#include "Server/LobbyGameModeBase.h"
#include "Server/AlkaidGameStateBase.h"
#include "MyPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

ALobbyGameModeBase::ALobbyGameModeBase()
{
	MinPlayers = 2;
	PlayerStateClass = AMyPlayerState::StaticClass();
	GameStateClass = AAlkaidGameStateBase::StaticClass();
	bUseSeamlessTravel = true;
}

void ALobbyGameModeBase::JoinRoom(AMyPlayerState* PS)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!PS)
	{
		return;
	}

	AAlkaidGameStateBase* GS = GetGameState<AAlkaidGameStateBase>();
	if (!GS)
	{
		UE_LOG(LogTemp, Warning, TEXT("JoinRoom: GS is null"));
		return;
	}

	if (GS->RoomState == ERoomState::Loading)
	{
		UE_LOG(LogTemp, Warning, TEXT("JoinRoom 거부: RoomState=Loading"));
		return;
	}

	if (PS->bInRoom)
	{
		UE_LOG(LogTemp, Warning, TEXT("JoinRoom 무시: 이미 InRoom"));
		return;
	}

	PS->bInRoom = true;
	PS->bReady = false;

	if (GS->RoomState == ERoomState::Free)
	{
		GS->RoomState = ERoomState::Match;
	}

	CheckStartReady();
	UE_LOG(LogTemp, Warning, TEXT("JoinRoom 완료: %s (RoomPlayers=%d, Ready=%d, StartReady=%d)"),
		*PS->GetPlayerName(),
		GS->RoomPlayerCount,
		GS->RoomReadyCount,
		GS->bStartReady ? 1 : 0);
}

void ALobbyGameModeBase::LeaveRoom(AMyPlayerState* PS)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!PS)
	{
		return;
	}

	AAlkaidGameStateBase* GS = GetGameState<AAlkaidGameStateBase>();
	if (!GS)
	{
		return;
	}

	if (!PS->bInRoom)
	{
		return;
	}

	PS->bInRoom = false;
	PS->bReady = false;

	EnsureRoomLeader();
	CheckStartReady();
}

void ALobbyGameModeBase::CheckStartReady()
{
	if (!HasAuthority())
	{
		return;
	}
	
	AAlkaidGameStateBase* GS = GetGameState<AAlkaidGameStateBase>();
	if (!GS)
	{
		return;
	}

	if (GS->RoomState == ERoomState::Loading)
	{
		return;
	}

	int32 RoomPlayers = 0;
	int32 ReadyPlayers = 0;
	const bool bAllReady = IsAllReadyInRoom(RoomPlayers, ReadyPlayers);

	const bool bCanStart = (GS->RoomState == ERoomState::Match) && (RoomPlayers >= MinPlayers) && bAllReady;

	GS->bStartReady = bCanStart;
	GS->RoomPlayerCount = RoomPlayers;
	GS->RoomReadyCount = ReadyPlayers;

	if (bCanStart)
	{
		TravelToPuzzle();
	}
}

void ALobbyGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	CheckStartReady();
}

void ALobbyGameModeBase::Logout(AController* Exiting)
{
	AMyPlayerState* ExitingPS = Exiting ? Exiting->GetPlayerState<AMyPlayerState>() : nullptr;

	Super::Logout(Exiting);

	if (ExitingPS)
	{
		ExitingPS->bInRoom = false;
		ExitingPS->bReady = false;
	}

	EnsureRoomLeader();
	CheckStartReady();
}

void ALobbyGameModeBase::EnsureRoomLeader()
{
	AAlkaidGameStateBase* GS = GetGameState<AAlkaidGameStateBase>();
	if (!GS)
	{
		return;
	}

	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (AMyPlayerState* MyPS = Cast<AMyPlayerState>(PS))
		{
			if (MyPS->bInRoom)
			{
				GS->RoomState = ERoomState::Match;
				return;
			}
		}
	}

	GS->RoomState = ERoomState::Free;
}

bool ALobbyGameModeBase::IsAllReadyInRoom(int32& OutRoomPlayers, int32& OutReadyPlayers) const
{
	OutRoomPlayers = 0;
	OutReadyPlayers = 0;

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}
	
	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			if (AMyPlayerState* PS = PC->GetPlayerState<AMyPlayerState>())
			{
				if (!PS->bInRoom)
				{
					continue;
				}
				OutRoomPlayers++;
				if (PS->bReady)
				{
					OutReadyPlayers++;
				}
			}
		}
	}
	return(OutRoomPlayers > 0 && OutReadyPlayers == OutRoomPlayers);
}


void ALobbyGameModeBase::TravelToPuzzle()
{
	if (!HasAuthority())
	{
		return;
	}

	if (AAlkaidGameStateBase* GS = GetGameState<AAlkaidGameStateBase>())
	{
		GS->RoomState = ERoomState::Loading;
		GS->bStartReady = false;
	}

	TravelTo(PuzzleMapPath);
}
