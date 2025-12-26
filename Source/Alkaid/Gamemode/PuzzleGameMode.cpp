#include "Gamemode/PuzzleGameMode.h"

#include "EngineUtils.h"
#include "Gamemode/PuzzleColorButton.h"
#include "Gamemode/PuzzleColorDoor.h"

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

	// 다음 퍼즐들은 여기에 추가
	// if (PuzzleKey == TEXT("1_2")) { HandlePuzzle_1_2(Button); return; }
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