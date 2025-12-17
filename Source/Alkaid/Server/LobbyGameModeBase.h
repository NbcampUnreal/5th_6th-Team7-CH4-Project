#pragma once

#include "CoreMinimal.h"
#include "Server/AlkaidGameModeBase.h"
#include "LobbyGameModeBase.generated.h"

UCLASS()
class ALKAID_API ALobbyGameModeBase : public AAlkaidGameModeBase
{
	GENERATED_BODY()
	
public:
	ALobbyGameModeBase();

	//최소 인원.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby")
	int32 MinPlayerCount;

	//Ready 여부 확인.
	UFUNCTION(BlueprintCallable)
	void CheckAllReady();

protected:
	//전원 준비 시 퍼즐 맵으로 이동.
	UFUNCTION()
	void StartGame();

};
