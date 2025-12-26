#pragma once

#include "CoreMinimal.h"
#include "PuzzleTypes.generated.h"

// 퍼즐에서 쓸 식별자 타입
// 버튼과 문을 이름으로 구분 ex): 1_1_RedButton, 1_1_BlueDoor
USTRUCT(BlueprintType)
struct FPuzzleId
{
	GENERATED_BODY()

public:
	// 실제 식별자 값, 에디터에서 인스턴스마다 입력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	FName Id = NAME_None;

	// 기본 생성자
	FPuzzleId() = default;

	// 값을 넣어서 만들 때 쓰는 생성자
	explicit FPuzzleId(const FName InId) : Id(InId) {}

	// 값이 비어있는지 확인
	// NAME_None이면 아직 지정하지 않은 상태
	FORCEINLINE bool IsValid() const { return Id != NAME_None; }
};

UENUM(BlueprintType)
enum class EPuzzleTriggerShape : uint8
{
	Box UMETA(DisplayName = "Box"),
	Capsule UMETA(DisplayName = "Capsule")
};