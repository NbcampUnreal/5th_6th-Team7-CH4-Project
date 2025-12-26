#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gamemode/PuzzleTypes.h"
#include "PuzzlePieceBase.generated.h"

class UStaticMeshComponent;

UCLASS()
class ALKAID_API APuzzlePieceBase : public AActor
{
	GENERATED_BODY()

public:
	APuzzlePieceBase();

	// 퍼즐조각 식별자 (표지판/리시버가 요구하는 ID와 매칭)
	UPROPERTY(EditInstanceOnly, Replicated, BlueprintReadOnly, Category = "Puzzle|Piece")
	FPuzzleId PieceId;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

protected:
	virtual void BeginPlay() override; 

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; 
};