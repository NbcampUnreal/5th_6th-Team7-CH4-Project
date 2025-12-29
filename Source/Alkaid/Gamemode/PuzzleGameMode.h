#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Server/AlkaidGameModeBase.h"
#include "PuzzleGameMode.generated.h"


class APuzzleColorButton;
class APuzzleColorDoor;

class APuzzleSignBase;
class APuzzleReceiverBox;
class APuzzleMoveHoleWall;

UCLASS()
class ALKAID_API APuzzleGameMode : public AAlkaidGameModeBase
{
	GENERATED_BODY()

public:
	APuzzleGameMode();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TMap<FName, TObjectPtr<APuzzleColorDoor>> DoorById;

	UPROPERTY()
	TMap<FName, TObjectPtr<APuzzleColorButton>> ButtonById;

	UPROPERTY()
	TSet<FName> OpenedDoorIds;

private:
	void CollectActorsAndBind();

	UFUNCTION()
	void OnButtonPressedChanged(APuzzleColorButton* Button, bool bPressed);

private:
	FString GetPuzzleKeyFromId(const FName& InId) const;
	FName MakeDoorIdFromButtonId_1_1(const FName& ButtonId) const;
	void OpenDoorById(const FName& DoorId);

private:
	void HandlePuzzle_1_1(APuzzleColorButton* Button);

private:
	// 1_2
	UPROPERTY()
	TMap<FName, TObjectPtr<APuzzleSignBase>> SignSlotById;

	UPROPERTY()
	TMap<FName, TObjectPtr<APuzzleReceiverBox>> ReceiverById;

	UPROPERTY()
	TSet<FName> SolvedReceiverIds_1_2;

	void InitPuzzle_1_2();

	UFUNCTION()
	void OnReceiverSolved(APuzzleReceiverBox* Receiver, bool bSolved);

	void CollectPuzzle12ActorsAndBind();

	//1_2에서도 BlueButton -> BlueDoor 처리(1_1 재활용)
	void HandlePuzzle_1_2(APuzzleColorButton* Button);


private:
	// 1_3 추가
	void CollectPuzzle13Actors();
	void HandlePuzzle_1_3(APuzzleColorButton* Button, bool bPressed);

private:
	// 1_3에서 움직일 구멍벽
	UPROPERTY()
	TObjectPtr<APuzzleMoveHoleWall> MoveHoleWall_1_3 = nullptr;

	// 두 버튼 동시/교차 입력까지 안전하게 상태 기억
	bool bPressed_L_1_3 = false;
	bool bPressed_R_1_3 = false;
};