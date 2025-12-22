#include "Server/AlkaidGameStateBase.h"
#include "Net/UnrealNetwork.h"

AAlkaidGameStateBase::AAlkaidGameStateBase()
{
	RoomState = ERoomState::Free;
	bStartReady = false;
	RoomPlayerCount = 0;
	RoomReadyCount = 0;
}

void AAlkaidGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAlkaidGameStateBase, RoomState);
	DOREPLIFETIME(AAlkaidGameStateBase, bStartReady);
	DOREPLIFETIME(AAlkaidGameStateBase, RoomPlayerCount);
	DOREPLIFETIME(AAlkaidGameStateBase, RoomReadyCount);
}

void AAlkaidGameStateBase::OnRep_RoomState()
{
	NotifyLobbyInfoChanged();
}

void AAlkaidGameStateBase::OnRep_StartReady()
{
	NotifyLobbyInfoChanged();

}

void AAlkaidGameStateBase::OnRep_RoomCount()
{
	NotifyLobbyInfoChanged();

}

void AAlkaidGameStateBase::NotifyLobbyInfoChanged()
{
	OnLobbyInfoChanged.Broadcast();
}
