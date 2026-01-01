#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AlkaidGameModeBase.generated.h"

UCLASS()
class ALKAID_API AAlkaidGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	//입장 맵.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Travel")
	FString LobbyMapPath = TEXT("125.183.170.242:7777");
	//퍼즐 맵.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Travel")
	FString PuzzleMapPath = TEXT("125.183.170.242:7778");
	//로비로 복귀.
	UFUNCTION(BlueprintCallable)
	void ReturnToLobby();

protected:
	void ResetRoomOnReturn();

	UPROPERTY(EditDefaultsOnly)
	bool bReloadLevelWhenEmpty = false;

	UPROPERTY(EditDefaultsOnly)
	FString ReloadLevelPath = TEXT("/Game/Map/Puzzle_map");

	bool bReloadIssued = false;

	virtual void Logout(AController* Exiting) override;

private:
	FTimerHandle ReloadCheckTimer;

	UFUNCTION()
	void TryReloadIfEmpty();
};
