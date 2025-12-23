#pragma once

#include "CoreMinimal.h"
#include "Server/AlkaidGameModeBase.h"
#include "LobbyGameModeBase.generated.h"

class AMyPlayerState;

UCLASS()
class ALKAID_API ALobbyGameModeBase : public AAlkaidGameModeBase
{
	GENERATED_BODY()
	
public:
	ALobbyGameModeBase();

	//최소 인원.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	int32 MinPlayers;

	void JoinRoom(AMyPlayerState* PS);
	void LeaveRoom(AMyPlayerState* PS);

	//Ready 여부 확인.
	UFUNCTION(BlueprintCallable, Category = "Room")
	void CheckStartReady();

	//void LeaderStart(AMyPlayerState* RequestPS);

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

protected:
	void EnsureRoomLeader();
	//void UpdateRoomCounts();

	bool IsAllReadyInRoom(int32& OutRoomPlayers, int32& OutReadyPlayers) const;
	//void AutoStart();
	
	void TravelToPuzzle();
};
