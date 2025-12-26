#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gamemode/PuzzleTypes.h"
#include "PuzzleColorDoor.generated.h"

class UStaticMeshComponent;

// 문 액터이며, 이 액터코드에서 작동조건이 없음
// 서버기준으로 작동하는 게임모드가 작동지시

UCLASS()
class ALKAID_API APuzzleColorDoor : public AActor
{
	GENERATED_BODY()

public:
	APuzzleColorDoor();

	// 게임모드가 호출하는 함수
	// 기본 동작은 열림=Destroy
	// 멀티에서는 서버에서만 호출

	UFUNCTION(BlueprintCallable, Category = "Puzzle|Door")
	void OpenDoor();

	// 문 식별자. ex: 1_1_BlueDoor
	// 레벨에 배치한 인스턴스마다 다르게 지정
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Puzzle|Door")
	FPuzzleId DoorId;

	// 문 메시
	// 외형은 블루프린트에서
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;
};