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
	FString LobbyMapPath = TEXT("/Game/Server/Map/lobby");
	//퍼즐 맵.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Travel")
	FString PuzzleMapPath = TEXT("/Game/Server/Map/추후예정");
	//로비로 복귀.
	UFUNCTION(BlueprintCallable)
	void ReturnToLobby();
	void TravelTo(const FString& MapPath);

protected:
	//맵 이동.
	void ResetRoomOnReturn();
};
