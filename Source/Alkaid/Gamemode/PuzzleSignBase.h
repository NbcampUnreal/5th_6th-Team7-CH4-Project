#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gamemode/PuzzleTypes.h"
#include "PuzzleSignBase.generated.h"

class UStaticMeshComponent;

UCLASS()
class ALKAID_API APuzzleSignBase : public AActor
{
	GENERATED_BODY()

public:
	APuzzleSignBase();

	// 슬롯 자체 ID(레벨에서 고정)
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Puzzle|Sign")
	FPuzzleId SignSlotId;

	// 이번 판에 표시할 ID(GameMode가 세팅)
	UPROPERTY(ReplicatedUsing = OnRep_DisplayId, BlueprintReadOnly, Category = "Puzzle|Sign")
	FPuzzleId DisplayId;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

protected:
	UFUNCTION()
	void OnRep_DisplayId();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// BP에서 모양/머티리얼 
	UFUNCTION(BlueprintImplementableEvent, Category = "Puzzle|Sign")
	void BP_OnDisplayIdChanged(FName NewId);
};