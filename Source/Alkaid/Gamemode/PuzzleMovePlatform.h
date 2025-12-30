#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gamemode/PuzzleTypes.h"
#include "PuzzleMovePlatform.generated.h"

class UStaticMeshComponent;
class USceneComponent;

UCLASS()
class ALKAID_API APuzzleMovePlatform : public AActor
{
	GENERATED_BODY()

public:
	APuzzleMovePlatform();

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Puzzle|Platform")
	FPuzzleId PlatformId; // 1_4_1_Pad ~ 1_4_4_Pad

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

	// Y 이동 범위
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puzzle|Platform")
	float MinY = 750.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puzzle|Platform")
	float MaxY = 2250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puzzle|Platform")
	float MoveSpeed = 250.f;

	// -1: MinY 방향, +1: MaxY 방향, 0: 정지
	UPROPERTY(BlueprintReadOnly, Category = "Puzzle|Platform")
	int32 MoveDir = 0;

	UFUNCTION(BlueprintCallable, Category = "Puzzle|Platform")
	void StartMoveLeft();

	UFUNCTION(BlueprintCallable, Category = "Puzzle|Platform")
	void StartMoveRight();

	UFUNCTION(BlueprintCallable, Category = "Puzzle|Platform")
	void StopMove();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};