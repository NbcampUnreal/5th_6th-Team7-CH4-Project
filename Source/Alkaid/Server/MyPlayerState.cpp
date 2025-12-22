#include "Server/MyPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "LobbyGameModeBase.h"

AMyPlayerState::AMyPlayerState()
{
	bReplicates = true;
	bInRoom = false;
	bReady = false;
}

void AMyPlayerState::ServerJoinRoom_Implementation()
{
	if (!HasAuthority())
	{
		return;
	}

	if (ALobbyGameModeBase* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ALobbyGameModeBase>() : nullptr)
	{
		GM->JoinRoom(this);
	}
}

void AMyPlayerState::ServerLeaveRoom_Implementation()
{
	if (!HasAuthority())
	{
		return;
	}

	if (ALobbyGameModeBase* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ALobbyGameModeBase>() : nullptr)
	{
		GM->LeaveRoom(this);
	}

}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyPlayerState, bInRoom);
	DOREPLIFETIME(AMyPlayerState, bReady);
}

void AMyPlayerState::ServerSetReady_Implementation(bool bNewReady)
{
	if (!HasAuthority())
	{
		return;
	}
	
	if (!bInRoom)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not in Room : %s"), *GetPlayerName());
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("ServerSetReady %s -> %s"), *GetPlayerName(), bNewReady ? TEXT("Ready") : TEXT("NotReady"));
	//상태 갱신.
	bReady = bNewReady;
	
	if (ALobbyGameModeBase* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ALobbyGameModeBase>() : nullptr)
	{
		GM->CheckStartReady();
	}
}
