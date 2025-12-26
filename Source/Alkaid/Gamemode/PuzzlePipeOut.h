#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gamemode/PuzzleTypes.h"
#include "PuzzlePipeOut.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class APuzzlePieceBase;

UCLASS()
class ALKAID_API APuzzlePipeOut : public AActor
{
	GENERATED_BODY()

public:
	APuzzlePipeOut();

	// PipeIn이 호출: 조각을 SpawnPoint 위치에서 생성
	UFUNCTION(BlueprintCallable, Category = "Puzzle|Pipe")
	APuzzlePieceBase* SpawnPiece(TSubclassOf<APuzzlePieceBase> PieceClass, FPuzzleId InPieceId);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

	//  파이프 "출구 아래쪽"에 배치 (중력으로 자연낙하)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SpawnPoint = nullptr;
};