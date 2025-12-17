#include "Server/MyPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameModeBase.h"

AMyPlayerState::AMyPlayerState()
{
	bReplicates = true;
	bIsReady = false;
}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//Ready 상태 동기화.
	DOREPLIFETIME(AMyPlayerState, bIsReady);
}

void AMyPlayerState::ServerSetReady_Implementation(bool bNewReady)
{
	UE_LOG(LogTemp, Warning, TEXT("ServerSetReady %s -> %s"), *GetPlayerName(), bNewReady ? TEXT("Ready") : TEXT("NotReady"));
	//상태 갱신.
	bIsReady = bNewReady;

	if (AGameModeBase* GM = GetWorld()->GetAuthGameMode())
	{
		if (ALobbyGameModeBase* LobbyGM = Cast<ALobbyGameModeBase>(GM))
		{
			LobbyGM->CheckAllReady();
		}
	}
}
