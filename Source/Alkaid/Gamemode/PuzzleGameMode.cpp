#include "Gamemode/PuzzleGameMode.h"

#include "EngineUtils.h"
#include "Gamemode/PuzzleColorButton.h"
#include "Gamemode/PuzzleColorDoor.h"

#include "Gamemode/PuzzleSignBase.h"
#include "Gamemode/PuzzleReceiverBox.h"

APuzzleGameMode::APuzzleGameMode()
{
}

void APuzzleGameMode::BeginPlay()
{
	Super::BeginPlay();

	// GameMode는 서버에만 존재하지만 안전하게 한번 더 검사
	if (!HasAuthority())
	{
		return;
	}

	CollectActorsAndBind();

	// 1_2 퍼즐 준비
	CollectPuzzle12ActorsAndBind();
	InitPuzzle_1_2();
}

void APuzzleGameMode::CollectActorsAndBind()
{
	DoorById.Empty();
	ButtonById.Empty();
	OpenedDoorIds.Empty();

	// 문 수집
	for (TActorIterator<APuzzleColorDoor> It(GetWorld()); It; ++It)
	{
		APuzzleColorDoor* Door = *It;
		if (!IsValid(Door))
		{
			continue;
		}

		const FName DoorId = Door->DoorId.Id;
		if (DoorId.IsNone())
		{
			continue;
		}

		DoorById.Add(DoorId, Door);
	}

	// 버튼 수집 및 바인딩
	for (TActorIterator<APuzzleColorButton> It(GetWorld()); It; ++It)
	{
		APuzzleColorButton* Button = *It;
		if (!IsValid(Button))
		{
			continue;
		}

		const FName ButtonId = Button->ButtonId.Id;
		if (ButtonId.IsNone())
		{
			continue;
		}

		ButtonById.Add(ButtonId, Button);

		// 중복 바인딩 방지 후 바인딩
		Button->OnPressedChanged.RemoveDynamic(this, &ThisClass::OnButtonPressedChanged);
		Button->OnPressedChanged.AddDynamic(this, &ThisClass::OnButtonPressedChanged);
	}
}

void APuzzleGameMode::OnButtonPressedChanged(APuzzleColorButton* Button, bool bPressed)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!IsValid(Button))
	{
		return;
	}

	// 1_1은 눌림이 true가 되는 순간만 처리
	if (!bPressed)
	{
		return;
	}

	const FName ButtonId = Button->ButtonId.Id;
	if (ButtonId.IsNone())
	{
		return;
	}

	const FString PuzzleKey = GetPuzzleKeyFromId(ButtonId);

	// 퍼즐을 계속 쌓아갈 분기 지점
	if (PuzzleKey == TEXT("1_1"))
	{
		HandlePuzzle_1_1(Button);
		return;
	}
}

FString APuzzleGameMode::GetPuzzleKeyFromId(const FName& InId) const
{
	const FString S = InId.ToString();

	TArray<FString> Parts;
	S.ParseIntoArray(Parts, TEXT("_"), true);

	// 1_1 처럼 최소 두 토큰이 있어야 함
	if (Parts.Num() < 2)
	{
		return FString();
	}

	return Parts[0] + TEXT("_") + Parts[1];
}

FName APuzzleGameMode::MakeDoorIdFromButtonId_1_1(const FName& ButtonId) const
{
	FString S = ButtonId.ToString();

	// 끝이 Button이면 Door로 바꾼다
	if (S.EndsWith(TEXT("Button")))
	{
		S.LeftChopInline(6);
		S += TEXT("Door");
	}

	return FName(*S);
}

void APuzzleGameMode::OpenDoorById(const FName& DoorId)
{
	if (DoorId.IsNone())
	{
		return;
	}

	// 이미 열린 문은 다시 처리하지 않음
	if (OpenedDoorIds.Contains(DoorId))
	{
		return;
	}

	const TObjectPtr<APuzzleColorDoor>* FoundPtr = DoorById.Find(DoorId);
	if (!FoundPtr)
	{
		return;
	}

	APuzzleColorDoor* Door = FoundPtr->Get();
	if (!IsValid(Door))
	{
		return;
	}

	OpenedDoorIds.Add(DoorId);

	// 문 내부에서 Destroy 한다
	Door->OpenDoor();
}

void APuzzleGameMode::HandlePuzzle_1_1(APuzzleColorButton* Button)
{
	if (!IsValid(Button))
	{
		return;
	}

	// 1_1 규칙
	// 버튼 Id에서 문 Id를 만든다
	const FName ButtonId = Button->ButtonId.Id;
	const FName DoorId = MakeDoorIdFromButtonId_1_1(ButtonId);

	OpenDoorById(DoorId);
}

// 1_2 로직

void APuzzleGameMode::CollectPuzzle12ActorsAndBind()
{
	SignSlotById.Empty();          
	ReceiverById.Empty();          
	SolvedReceiverIds_1_2.Empty(); 

	// SignSlot 수집
	for (TActorIterator<APuzzleSignBase> It(GetWorld()); It; ++It) 
	{
		APuzzleSignBase* Sign = *It;
		if (!IsValid(Sign))
		{
			continue;
		}

		const FName SlotId = Sign->SignSlotId.Id;
		if (SlotId.IsNone())
		{
			continue;
		}

		// 1_2_SignSlot 로 시작하는 것만 사용(다른 퍼즐 대비)
		if (!SlotId.ToString().StartsWith(TEXT("1_2_SignSlot")))
		{
			continue;
		}

		SignSlotById.Add(SlotId, Sign);
	}

	// Receiver 수집 및 이벤트 바인딩
	for (TActorIterator<APuzzleReceiverBox> It(GetWorld()); It; ++It) 
	{
		APuzzleReceiverBox* Receiver = *It;
		if (!IsValid(Receiver))
		{
			continue;
		}

		const FName Rid = Receiver->ReceiverId.Id;
		if (Rid.IsNone())
		{
			continue;
		}

		if (!Rid.ToString().StartsWith(TEXT("1_2_Receiver")))
		{
			continue;
		}

		ReceiverById.Add(Rid, Receiver);

		Receiver->OnSolved.RemoveDynamic(this, &ThisClass::OnReceiverSolved); 
		Receiver->OnSolved.AddDynamic(this, &ThisClass::OnReceiverSolved);    
	}
}

void APuzzleGameMode::InitPuzzle_1_2()
{
	//필수 3개가 없으면 진행 불가
	if (SignSlotById.Num() < 3 || ReceiverById.Num() < 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Puzzle 1_2: Need 3 SignSlots and 3 Receivers."));
		return;
	}

	//6개 후보 ID 풀
	TArray<FName> Candidates;
	for (int32 i = 1; i <= 6; ++i)
	{
		Candidates.Add(FName(*FString::Printf(TEXT("1_2_Piece_%d"), i)));
	}

	//중복 없이 3개 선택
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

	// 슬롯/리시버를 “정렬된 키” 기준으로 안정적으로 매칭
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

		if (!IsValid(Slot) || !IsValid(Receiver))
		{
			continue;
		}

		// 서버에서 DisplayId / ExpectedId 세팅
		Slot->ServerSetDisplayId(FPuzzleId(Picked[i]));  // 서버 세팅

		Receiver->ExpectedId = FPuzzleId(Picked[i]);   
		Receiver->bSolved = false;                     // 초기화(이전값 방지)
	}

	SolvedReceiverIds_1_2.Empty();
}

void APuzzleGameMode::OnReceiverSolved(APuzzleReceiverBox* Receiver, bool bSolved)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!IsValid(Receiver))
	{
		return;
	}

	const FName Rid = Receiver->ReceiverId.Id;
	if (Rid.IsNone())
	{
		return;
	}

	// 1_2 리시버만 처리
	if (!Rid.ToString().StartsWith(TEXT("1_2_Receiver")))
	{
		return;
	}

	if (bSolved)
	{
		SolvedReceiverIds_1_2.Add(Rid); // 
	}
	else
	{
		SolvedReceiverIds_1_2.Remove(Rid); // (안전)
	}

	// 3개 solved면 문 열기
	if (SolvedReceiverIds_1_2.Num() >= 3)
	{
		OpenDoorById(FName(TEXT("1_2_YellowDoor"))); 
	}
}