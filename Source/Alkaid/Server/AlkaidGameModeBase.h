#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AlkaidGameModeBase.generated.h"

UCLASS()
class ALKAID_API AAlkaidGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	//메인 맵.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Travel")
	FString LobbyMapPath = TEXT("/Game/Server/Map/TestMap");
	//퍼즐 맵.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Travel")
	FString PuzzleMapPath = TEXT("/Game/Server/Map/TestMap1");
	//로비로 복귀.
	UFUNCTION(BlueprintCallable)
	void ReturnToLobby();
	//재시작(퍼즐 맵).
	UFUNCTION(BlueprintCallable)
	void RetryGame();

protected:
	//맵 이동.
	void TravelTo(const FString& MapPath);
};
