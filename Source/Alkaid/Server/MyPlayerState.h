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

	//준비 상태.
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ready")
	bool bIsReady;

	//UI에서 호출 -> 서버가 Ready 상태를 갱신.
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerSetReady(bool bNewReady);

	//Replicated 변수 등록.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
