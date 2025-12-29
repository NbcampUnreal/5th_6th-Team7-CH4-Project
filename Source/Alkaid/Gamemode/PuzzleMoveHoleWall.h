#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gamemode/PuzzleTypes.h"
#include "PuzzleMoveHoleWall.generated.h"

class UStaticMeshComponent;
class USceneComponent;

UCLASS()
class ALKAID_API APuzzleMoveHoleWall : public AActor
{
	GENERATED_BODY()

public:
	APuzzleMoveHoleWall();

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Puzzle|Wall")
	FPuzzleId WallId;

	// Y 이동 범위
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puzzle|Wall")
	float MinY = -1250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puzzle|Wall")
	float MaxY = -350.f;

	// 이동 속도(uu/s)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puzzle|Wall")
	float MoveSpeed = 250.f;

	// -1: MinY 방향, +1: MaxY 방향, 0: 정지
	UPROPERTY(ReplicatedUsing = OnRep_MoveDir, BlueprintReadOnly, Category = "Puzzle|Wall")
	int32 MoveDir = 0;

	// 서버에서만 호출(버튼 눌림/해제에 대응)
	void StartMoveLeft();   // MinY 쪽으로
	void StartMoveRight();  // MaxY 쪽으로
	void StopMove();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnRep_MoveDir();

	void ApplyTickEnabledFromDir();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;
};