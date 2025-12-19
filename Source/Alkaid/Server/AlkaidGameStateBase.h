#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AlkaidGameStateBase.generated.h"

UENUM(BlueprintType)
enum class ERoomState : uint8
{
	Free UMETA(DisplayName = "Free"),
	Match UMETA(DisplayName = "Match"),
	Loading UMETA(DisplayName = "Loading"),
};

UCLASS()
class ALKAID_API AAlkaidGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AAlkaidGameStateBase();

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Room")
	ERoomState RoomState;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Room")
	APlayerState* RoomLeaderPS;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Room")
	bool bStartReady;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Room")
	int32 RoomPlayerCount;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Room")
	int32 RoomReadyCount;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
