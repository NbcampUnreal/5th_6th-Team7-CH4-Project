#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PuzzleGameMode.generated.h"

class APuzzleColorButton;
class APuzzleColorDoor;

UCLASS()
class ALKAID_API APuzzleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APuzzleGameMode();

protected:
	virtual void BeginPlay() override;

private:
	// 레벨에 있는 문을 Id로 빠르게 찾기 위한 맵 
	// TMap(키-값 저장소)
	UPROPERTY()
	TMap<FName, TObjectPtr<APuzzleColorDoor>> DoorById;

	// 레벨에 있는 버튼을 Id로 빠르게 찾기 위한 맵
	UPROPERTY()
	TMap<FName, TObjectPtr<APuzzleColorButton>> ButtonById;

	// 이미 연 문은 다시 열지 않기 위한 기록
	UPROPERTY()
	TSet<FName> OpenedDoorIds;

private:
	// 레벨에 배치된 버튼과 문을 수집하고 바인딩
	void CollectActorsAndBind();

	// 버튼 눌림 이벤트를 받는 함수
	UFUNCTION()
	void OnButtonPressedChanged(APuzzleColorButton* Button, bool bPressed);

private:
	// 버튼 Id에서 퍼즐 키를 뽑는다
	// 예: 1_1_BlueButton -> 1_1
	FString GetPuzzleKeyFromId(const FName& InId) const;

	// 버튼 Id에서 문 Id를 만든다
	// 예: 1_1_BlueButton -> 1_1_BlueDoor
	FName MakeDoorIdFromButtonId_1_1(const FName& ButtonId) const;

	// 문을 Id로 찾아 열기
	void OpenDoorById(const FName& DoorId);

private:
	// 1_1 퍼즐 규칙
	void HandlePuzzle_1_1(APuzzleColorButton* Button);
};