#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

UCLASS()
class ALKAID_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AMyPlayerState();

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Room")
	bool bInRoom;
	//준비 상태.
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Room")
	bool bReady;

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Room")
	void ServerJoinRoom();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Room")
	void ServerLeaveRoom();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Room")
	void ServerSetReady(bool bNewReady);

	//Replicated 변수 등록.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
