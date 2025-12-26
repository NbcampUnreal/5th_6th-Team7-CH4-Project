#include "Server/AlkaidGameStateBase.h"
#include "Net/UnrealNetwork.h"

AAlkaidGameStateBase::AAlkaidGameStateBase()
{
	RoomState = ERoomState::Free;
	RoomLeaderPS = nullptr;
	bStartReady = false;
	RoomPlayerCount = 0;
	RoomReadyCount = 0;
}

void AAlkaidGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAlkaidGameStateBase, RoomState)
	DOREPLIFETIME(AAlkaidGameStateBase, RoomLeaderPS)
	DOREPLIFETIME(AAlkaidGameStateBase, bStartReady)
	DOREPLIFETIME(AAlkaidGameStateBase, RoomPlayerCount)
	DOREPLIFETIME(AAlkaidGameStateBase, RoomReadyCount)
}
