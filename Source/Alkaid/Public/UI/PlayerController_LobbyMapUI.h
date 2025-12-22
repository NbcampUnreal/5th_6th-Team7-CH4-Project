#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerController_LobbyMapUI.generated.h"

class UUserWidget;
class USoundBase;

UCLASS()
class ALKAID_API APlayerController_LobbyMapUI : public APlayerController
{
	GENERATED_BODY()

public:
  
    UPROPERTY(EditAnywhere, Category = "Lobby")
    TSubclassOf<class UUserWidget> LobbyWidgetClass;

    UPROPERTY(EditAnywhere, Category = "Lobby")
    class USoundBase* LobbySound;

    UFUNCTION(BlueprintCallable, Category = "LobbyUI")
    void RequestTravelToLobby();

    UFUNCTION(Server, Reliable)
    void Server_TravelToLobby();

protected:
    virtual void BeginPlay() override;
    // 카메라 포커스 설정
    virtual void OnPossess(APawn* pawn) override;
    // 클라이언트에서 각 카메라 세팅 적용
    UFUNCTION(Client, Reliable)
    void Client_SetupLobby();

private:
    UPROPERTY()
    class UUserWidget* LobbyWidgetInstance;
    void LobbySettings();
};

