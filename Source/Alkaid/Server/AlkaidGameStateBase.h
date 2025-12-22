#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AlkaidGameStateBase.generated.h"

class AMyPlayerState;

UENUM(BlueprintType)
enum class ERoomState : uint8
{
	Free UMETA(DisplayName = "Free"),
	Match UMETA(DisplayName = "Match"),
	Loading UMETA(DisplayName = "Loading"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyInfoChanged);

UCLASS()
class ALKAID_API AAlkaidGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AAlkaidGameStateBase();

	UPROPERTY(BlueprintAssignable, Category = "Room")
	FOnLobbyInfoChanged OnLobbyInfoChanged;

	UPROPERTY(ReplicatedUsing = OnRep_RoomState, BlueprintReadOnly, Category = "Room")
	ERoomState RoomState;
	
	UPROPERTY(ReplicatedUsing = OnRep_StartReady, BlueprintReadOnly, Category = "Room")
	bool bStartReady;
	
	UPROPERTY(ReplicatedUsing = OnRep_RoomCount, BlueprintReadOnly, Category = "Room")
	int32 RoomPlayerCount;
	
	UPROPERTY(ReplicatedUsing = OnRep_RoomCount, BlueprintReadOnly, Category = "Room")
	int32 RoomReadyCount;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	UFUNCTION()
	void OnRep_RoomState();
	UFUNCTION()
	void OnRep_StartReady();
	UFUNCTION()
	void OnRep_RoomCount();

private:

	void NotifyLobbyInfoChanged();
};
