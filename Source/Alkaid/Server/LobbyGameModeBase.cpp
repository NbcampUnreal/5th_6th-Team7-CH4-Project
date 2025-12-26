#include "Server/LobbyGameModeBase.h"
#include "Server/AlkaidGameStateBase.h"
#include "MyPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
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

	int32 RoomPlayers = 0;
	int32 ReadyPlayers = 0;
	IsAllReadyInRoom(RoomPlayers, ReadyPlayers);

	const bool bCanStart = (GS->RoomState == ERoomState::Match) && (ReadyPlayers >= MinPlayers);

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
	if (AMyPlayerState* ExitingPS = Exiting ? Exiting->GetPlayerState<AMyPlayerState>() : nullptr)
	{
		ExitingPS->bInRoom = false;
		ExitingPS->bReady = false;
	}

	Super::Logout(Exiting);

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
	
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	TArray<APlayerController*> ReadyPC;

	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC)
		{
			continue;
		}

		AMyPlayerState* MyPS = PC->GetPlayerState<AMyPlayerState>();
		if (!MyPS)
		{
			continue;
		}
		if (MyPS->bInRoom && MyPS->bReady)
		{
			ReadyPC.Add(PC);
		}
	}
	if (ReadyPC.Num() < MinPlayers)
	{
		return;
	}

	ReadyPC.Sort([](const APlayerController& A, const APlayerController& B)
	{
		const APlayerState* PSA = A.PlayerState;
		const APlayerState* PSB = B.PlayerState;
		const int32 IdA = PSA ? PSA->GetPlayerId() : INT32_MAX;
		const int32 IdB = PSB ? PSB->GetPlayerId() : INT32_MAX;
		return IdA < IdB;
	});

	TArray<APlayerController*> ToTravel;
	ToTravel.Reserve(MinPlayers);
	for (int32 i = 0; i < MinPlayers; ++i)
	{
		ToTravel.Add(ReadyPC[i]);
	}

	for (APlayerController* PC : ToTravel)
	{
		if (AMyPlayerState* MyPS = PC->GetPlayerState<AMyPlayerState>())
		{
			MyPS->bInRoom = false;
			MyPS->bReady = false;
		}
	}

	EnsureRoomLeader();
	CheckStartReady();

	for (APlayerController* PC : ToTravel)
	{
		PC->ClientTravel(PuzzleMapPath, ETravelType::TRAVEL_Absolute);
	}
}
