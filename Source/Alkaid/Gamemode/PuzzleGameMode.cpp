#include "Gamemode/PuzzleGameMode.h"

#include "EngineUtils.h"
#include "Gamemode/PuzzleColorButton.h"
#include "Gamemode/PuzzleColorDoor.h"

#include "Gamemode/PuzzleSignBase.h"
#include "Gamemode/PuzzleReceiverBox.h"
#include "Gamemode/PuzzleMoveHoleWall.h"
#include "Gamemode/PuzzleMovePlatform.h"
#include "Gamemode/PuzzlePressObject.h"

APuzzleGameMode::APuzzleGameMode()
{
}

void APuzzleGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	CollectActorsAndBind();

	// 1_2 퍼즐 준비
	CollectPuzzle12ActorsAndBind();
	InitPuzzle_1_2();

	// 1_3 퍼즐 준비
	CollectPuzzle13Actors();

	// 1_4 퍼즐 준비
	CollectPuzzle14Actors();

	// 2_ 부터는, 버튼 접촉시 즉발이 아닌 상태머신(and계산)이 생겼으므로 안전처리
	bPressed_Red_2_1 = false;
	bPressed_Blue_2_1 = false;
	bSolved_2_1 = false;

	bPressed_Red_2_2 = false;
	bPressed_Green_2_2 = false;
	bPressed_Blue_2_2 = false;
	bSolved_2_2 = false;
}

void APuzzleGameMode::CollectActorsAndBind()
{
	DoorById.Empty();
	ButtonById.Empty();
	OpenedDoorIds.Empty();

	for (TActorIterator<APuzzleColorDoor> It(GetWorld()); It; ++It)
	{
		APuzzleColorDoor* Door = *It;
		if (!IsValid(Door)) continue;

		const FName DoorId = Door->DoorId.Id;
		if (DoorId.IsNone()) continue;

		DoorById.Add(DoorId, Door);
	}

	for (TActorIterator<APuzzleColorButton> It(GetWorld()); It; ++It)
	{
		APuzzleColorButton* Button = *It;
		if (!IsValid(Button)) continue;

		const FName ButtonId = Button->ButtonId.Id;
		if (ButtonId.IsNone()) continue;

		ButtonById.Add(ButtonId, Button);

		Button->OnPressedChanged.RemoveDynamic(this, &ThisClass::OnButtonPressedChanged);
		Button->OnPressedChanged.AddDynamic(this, &ThisClass::OnButtonPressedChanged);
	}
}

void APuzzleGameMode::OnButtonPressedChanged(APuzzleColorButton* Button, bool bPressed)
{
	if (!HasAuthority()) return;
	if (!IsValid(Button)) return;

	const FName ButtonId = Button->ButtonId.Id;
	if (ButtonId.IsNone()) return;

	const FString PuzzleKey = GetPuzzleKeyFromId(ButtonId);

	// 여기서부터“퍼즐별” 처리
	if (PuzzleKey == TEXT("1_1"))
	{
		// 1_1은 눌림이 true 되는 순간만 처리(기존 유지)
		if (bPressed)
		{
			HandlePuzzle_1_1(Button);
		}
		return;
	}

	if (PuzzleKey == TEXT("1_2"))
	{
		if (bPressed)
		{
			HandlePuzzle_1_2(Button);
		}
		return;
	}

	if (PuzzleKey == TEXT("1_3"))
	{
		// 1_3은 true/false 둘 다 처리해야 함
		HandlePuzzle_1_3(Button, bPressed);
		return;
	}

	if (PuzzleKey == TEXT("1_4"))
	{
		HandlePuzzle_1_4(Button, bPressed); // true/false 둘 다 처리
		return;
	}

	if (PuzzleKey == TEXT("2_1"))
	{
		HandlePuzzle_2_1(Button, bPressed); // true/false 둘 다 처리
		return;
	}

	if (PuzzleKey == TEXT("2_2"))
	{
		HandlePuzzle_2_2(Button, bPressed); // true/false 둘 다 처리
		return;
	}
}

FString APuzzleGameMode::GetPuzzleKeyFromId(const FName& InId) const
{
	const FString S = InId.ToString();

	TArray<FString> Parts;
	S.ParseIntoArray(Parts, TEXT("_"), true);

	if (Parts.Num() < 2)
	{
		return FString();
	}

	return Parts[0] + TEXT("_") + Parts[1];
}

FName APuzzleGameMode::MakeDoorIdFromButtonId_1_1(const FName& ButtonId) const
{
	FString S = ButtonId.ToString();

	if (S.EndsWith(TEXT("Button")))
	{
		S.LeftChopInline(6);
		S += TEXT("Door");
	}

	return FName(*S);
}

void APuzzleGameMode::OpenDoorById(const FName& DoorId)
{
	if (DoorId.IsNone()) return;

	if (OpenedDoorIds.Contains(DoorId)) return;

	const TObjectPtr<APuzzleColorDoor>* FoundPtr = DoorById.Find(DoorId);
	if (!FoundPtr) return;

	APuzzleColorDoor* Door = FoundPtr->Get();
	if (!IsValid(Door)) return;

	OpenedDoorIds.Add(DoorId);
	Door->OpenDoor();
}

void APuzzleGameMode::HandlePuzzle_1_1(APuzzleColorButton* Button)
{
	if (!IsValid(Button)) return;

	const FName ButtonId = Button->ButtonId.Id;
	const FName DoorId = MakeDoorIdFromButtonId_1_1(ButtonId);

	OpenDoorById(DoorId);
}


// 1_2 로직 

void APuzzleGameMode::HandlePuzzle_1_2(APuzzleColorButton* Button)
{
	if (!IsValid(Button)) return;

	const FName ButtonId = Button->ButtonId.Id;
	const FString IdStr = ButtonId.ToString();

	// 1_2에서 BlueButton만 처리 (다른 버튼이 있으면 확장 가능)
	if (IdStr == TEXT("1_2_BlueButton"))
	{
		const FName DoorId = MakeDoorIdFromButtonId_1_1(Button->ButtonId.Id);
		OpenDoorById(DoorId);
	}
}

void APuzzleGameMode::CollectPuzzle12ActorsAndBind()
{
	SignSlotById.Empty();
	ReceiverById.Empty();
	SolvedReceiverIds_1_2.Empty();

	for (TActorIterator<APuzzleSignBase> It(GetWorld()); It; ++It)
	{
		APuzzleSignBase* Sign = *It;
		if (!IsValid(Sign)) continue;

		const FName SlotId = Sign->SignSlotId.Id;
		if (SlotId.IsNone()) continue;

		if (!SlotId.ToString().StartsWith(TEXT("1_2_SignSlot"))) continue;

		SignSlotById.Add(SlotId, Sign);
	}

	for (TActorIterator<APuzzleReceiverBox> It(GetWorld()); It; ++It)
	{
		APuzzleReceiverBox* Receiver = *It;
		if (!IsValid(Receiver)) continue;

		const FName Rid = Receiver->ReceiverId.Id;
		if (Rid.IsNone()) continue;

		if (!Rid.ToString().StartsWith(TEXT("1_2_Receiver"))) continue;

		ReceiverById.Add(Rid, Receiver);

		Receiver->OnSolved.RemoveDynamic(this, &ThisClass::OnReceiverSolved);
		Receiver->OnSolved.AddDynamic(this, &ThisClass::OnReceiverSolved);
	}
}

void APuzzleGameMode::InitPuzzle_1_2()
{
	if (SignSlotById.Num() < 3 || ReceiverById.Num() < 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Puzzle 1_2: Need 3 SignSlots and 3 Receivers."));
		return;
	}

	TArray<FName> Candidates;
	for (int32 i = 1; i <= 6; ++i)
	{
		Candidates.Add(FName(*FString::Printf(TEXT("1_2_Piece_%d"), i)));
	}

	TArray<FName> Picked;
	while (Picked.Num() < 3 && Candidates.Num() > 0)
	{
		const int32 Idx = FMath::RandRange(0, Candidates.Num() - 1);
		Picked.Add(Candidates[Idx]);
		Candidates.RemoveAtSwap(Idx);
	}

	if (Picked.Num() != 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Puzzle 1_2: Failed to pick 3 ids."));
		return;
	}

	TArray<FName> SlotKeys;
	SignSlotById.GetKeys(SlotKeys);
	SlotKeys.Sort(FNameLexicalLess());

	TArray<FName> ReceiverKeys;
	ReceiverById.GetKeys(ReceiverKeys);
	ReceiverKeys.Sort(FNameLexicalLess());

	for (int32 i = 0; i < 3; ++i)
	{
		APuzzleSignBase* Slot = SignSlotById[SlotKeys[i]].Get();
		APuzzleReceiverBox* Receiver = ReceiverById[ReceiverKeys[i]].Get();

		if (!IsValid(Slot) || !IsValid(Receiver)) continue;

		Slot->ServerSetDisplayId(FPuzzleId(Picked[i]));

		Receiver->ExpectedId = FPuzzleId(Picked[i]);
		Receiver->bSolved = false;
	}

	SolvedReceiverIds_1_2.Empty();
}

void APuzzleGameMode::OnReceiverSolved(APuzzleReceiverBox* Receiver, bool bSolved)
{
	if (!HasAuthority()) return;
	if (!IsValid(Receiver)) return;

	const FName Rid = Receiver->ReceiverId.Id;
	if (Rid.IsNone()) return;

	if (!Rid.ToString().StartsWith(TEXT("1_2_Receiver"))) return;

	if (bSolved) SolvedReceiverIds_1_2.Add(Rid);
	else        SolvedReceiverIds_1_2.Remove(Rid);

	if (SolvedReceiverIds_1_2.Num() >= 3)
	{
		OpenDoorById(FName(TEXT("1_2_YellowDoor")));
	}
}

// 1_3 로직

void APuzzleGameMode::CollectPuzzle13Actors()
{
	MoveHoleWall_1_3 = nullptr;

	// 레벨에 1개만 있다고 가정
	for (TActorIterator<APuzzleMoveHoleWall> It(GetWorld()); It; ++It)
	{
		APuzzleMoveHoleWall* Wall = *It;
		if (!IsValid(Wall)) continue;

		MoveHoleWall_1_3 = Wall;
		break;
	}

	if (!IsValid(MoveHoleWall_1_3))
	{
		UE_LOG(LogTemp, Warning, TEXT("Puzzle 1_3: MoveHoleWall actor not found in level."));
	}

	// 버튼 상태 초기화
	bPressed_L_1_3 = false;
	bPressed_R_1_3 = false;

	// 시작 시 정지 상태 보장
	if (IsValid(MoveHoleWall_1_3))
	{
		MoveHoleWall_1_3->StopMove();
	}
}

void APuzzleGameMode::HandlePuzzle_1_3(APuzzleColorButton* Button, bool bPressed)
{
	if (!IsValid(Button)) return;
	if (!IsValid(MoveHoleWall_1_3))
	{
		UE_LOG(LogTemp, Warning, TEXT("Puzzle 1_3: MoveHoleWall is null."));
		return;
	}

	const FName ButtonId = Button->ButtonId.Id;
	const FString IdStr = ButtonId.ToString();

	if (IdStr == TEXT("1_3_BlueButton"))
	{
		if (bPressed)
		{
			OpenDoorById(FName(TEXT("1_3_BlueDoor")));
		}
		return;
	}

	const bool bIsLeft = (IdStr == TEXT("1_3_L_Button"));
	const bool bIsRight = (IdStr == TEXT("1_3_R_Button"));

	if (!bIsLeft && !bIsRight)
	{
		// 1_3이긴 한데 다른 버튼이면 무시
		return;
	}

	// 눌림 상태 업데이트
	if (bIsLeft)  bPressed_L_1_3 = bPressed;
	if (bIsRight) bPressed_R_1_3 = bPressed;

	// 최종 동작 결정(안전 처리)
	// - 한쪽만 눌림: 그 방향으로 이동
	// - 둘 다 눌림(또는 둘 다 해제): 정지
	if (bPressed_L_1_3 && !bPressed_R_1_3)
	{
		MoveHoleWall_1_3->StartMoveLeft();   // MinY 방향
	}
	else if (bPressed_R_1_3 && !bPressed_L_1_3)
	{
		MoveHoleWall_1_3->StartMoveRight();  // MaxY 방향
	}
	else
	{
		MoveHoleWall_1_3->StopMove();
	}
}

void APuzzleGameMode::CollectPuzzle14Actors()
{
	PlatformById_1_4.Empty();
	PressedLeftByPad_1_4.Empty();
	PressedRightByPad_1_4.Empty();

	for (TActorIterator<APuzzleMovePlatform> It(GetWorld()); It; ++It)
	{
		APuzzleMovePlatform* Pad = *It;
		if (!IsValid(Pad)) continue;

		const FName PadId = Pad->PlatformId.Id;
		if (PadId.IsNone()) continue;

		const FString S = PadId.ToString();
		if (!S.StartsWith(TEXT("1_4_"))) continue;
		if (!S.EndsWith(TEXT("_Pad"))) continue;

		PlatformById_1_4.Add(PadId, Pad);
		PressedLeftByPad_1_4.Add(PadId, false);
		PressedRightByPad_1_4.Add(PadId, false);

		Pad->StopMove();
	}
}

bool APuzzleGameMode::ParseButtonToPad_1_4(const FName& ButtonId, FName& OutPadId, bool& bOutLeft) const
{
	const FString S = ButtonId.ToString();
	TArray<FString> Parts;
	S.ParseIntoArray(Parts, TEXT("_"), true);

	if (Parts.Num() != 5) return false;
	if (Parts[0] != TEXT("1")) return false;
	if (Parts[1] != TEXT("4")) return false;

	const FString& NumToken = Parts[2];     // "1"~"4"
	const FString& DirToken = Parts[3];     // "L" or "R"
	const FString& LastToken = Parts[4];    // "Button"

	if (LastToken != TEXT("Button")) return false;
	if (DirToken != TEXT("L") && DirToken != TEXT("R")) return false;

	bOutLeft = (DirToken == TEXT("L"));

	const FString PadStr = FString::Printf(TEXT("1_4_%s_Pad"), *NumToken);
	OutPadId = FName(*PadStr);
	return true;
}

void APuzzleGameMode::HandlePuzzle_1_4(APuzzleColorButton* Button, bool bPressed)
{
	if (!IsValid(Button)) return;

	const FString IdStr = Button->ButtonId.Id.ToString();
	if (IdStr == TEXT("1_4_RedButton"))
	{
		if (bPressed)
		{
			OpenDoorById(FName(TEXT("1_4_RedDoor")));
		}
		return;
	}

	FName PadId;
	bool bLeft = false;

	if (!ParseButtonToPad_1_4(Button->ButtonId.Id, PadId, bLeft))
	{
		return;
	}

	const TObjectPtr<APuzzleMovePlatform>* Found = PlatformById_1_4.Find(PadId);
	if (!Found) return;

	APuzzleMovePlatform* Pad = Found->Get();
	if (!IsValid(Pad)) return;

	// 눌림 상태 갱신
	if (bLeft)  PressedLeftByPad_1_4.FindOrAdd(PadId) = bPressed;
	else        PressedRightByPad_1_4.FindOrAdd(PadId) = bPressed;

	const bool bL = PressedLeftByPad_1_4.FindRef(PadId);
	const bool bR = PressedRightByPad_1_4.FindRef(PadId);

	// 한쪽만 눌림 -> 이동, 그 외(둘 다/둘 다 아님) -> 정지
	if (bL && !bR)      Pad->StartMoveLeft();
	else if (bR && !bL) Pad->StartMoveRight();
	else                Pad->StopMove();
}

void APuzzleGameMode::HandlePuzzle_2_1(APuzzleColorButton* Button, bool bPressed)
{
	if (!IsValid(Button)) return;

	// 한 번 열리면 끝(영구 오픈)
	if (bSolved_2_1) return;

	const FString IdStr = Button->ButtonId.Id.ToString();

	if (IdStr == TEXT("2_1_RedButton"))
	{
		bPressed_Red_2_1 = bPressed;
	}
	else if (IdStr == TEXT("2_1_BlueButton"))
	{
		bPressed_Blue_2_1 = bPressed;
	}
	else
	{
		return; // 2_1이지만 다른 버튼이면 무시
	}

	// 둘 다 눌린 상태가 되는 순간 문 오픈(1회)
	if (bPressed_Red_2_1 && bPressed_Blue_2_1)
	{
		bSolved_2_1 = true;
		OpenDoorById(FName(TEXT("2_1_YellowDoor")));
	}
}

void APuzzleGameMode::HandlePuzzle_2_2(APuzzleColorButton* Button, bool bPressed)
{
	if (!IsValid(Button)) return;

	// 한 번 열리면 끝(영구 오픈)
	if (bSolved_2_2) return;

	const FString IdStr = Button->ButtonId.Id.ToString();

	if (IdStr == TEXT("2_2_RedButton"))
	{
		bPressed_Red_2_2 = bPressed;
	}
	else if (IdStr == TEXT("2_2_GreenButton"))
	{
		bPressed_Green_2_2 = bPressed;
	}
	else if (IdStr == TEXT("2_2_BlueButton"))
	{
		bPressed_Blue_2_2 = bPressed;
	}
	else
	{
		return; // 2_2이지만 다른 버튼이면 무시
	}

	// 3개 모두 눌린 순간 오픈(1회)
	if (bPressed_Red_2_2 && bPressed_Green_2_2 && bPressed_Blue_2_2)
	{
		bSolved_2_2 = true;
		OpenDoorById(FName(TEXT("2_2_YellowDoor")));

		DestroyPressObjectsForPuzzle(FName(TEXT("2_2")));
	}
}

void APuzzleGameMode::DestroyPressObjectsForPuzzle(const FName& PuzzleKey)
{
	if (!HasAuthority()) return;

	for (TActorIterator<APuzzlePressObject> It(GetWorld()); It; ++It)
	{
		APuzzlePressObject* Obj = *It;
		if (!IsValid(Obj)) continue;

		if (Obj->OwnerPuzzleId.Id == PuzzleKey)
		{
			Obj->Destroy();
		}
	}
}