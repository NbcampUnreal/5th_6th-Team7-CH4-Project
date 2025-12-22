#include "UI/Interaction/CommonUserWidget_InteractionText.h"
#include "Components/TextBlock.h"
#include "Server/AlkaidGameStateBase.h"
#include "Server/MyPlayerState.h"
#include "Server/LobbyGameModeBase.h"

void UCommonUserWidget_InteractionText::NativeConstruct()
{
	Super::NativeConstruct();

	UpdatePlayerStates();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&UCommonUserWidget_InteractionText::UpdatePlayerStates,
		0.2f,
		true
	);
}

void UCommonUserWidget_InteractionText::NativeDestruct()
{
	Super::NativeDestruct();
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void UCommonUserWidget_InteractionText::UpdatePlayerStates()
{
    int32 Total = 2;
    int32 Ready = 0;

    if (!TextBlock_Ready) return;

    AAlkaidGameStateBase* PlayerCount = GetWorld()->GetGameState<AAlkaidGameStateBase>();

    if (!PlayerCount) return;

    for (APlayerState* CountState : PlayerCount->PlayerArray)
    {
        if (AMyPlayerState* MyCountState = Cast<AMyPlayerState>(CountState))
        {
            if (MyCountState->bInRoom && MyCountState->bReady)
            {
                Ready++;
            }
        }
    }
    FString ReadyText = FString::Printf(TEXT("%d / %d"), Ready, Total);
    TextBlock_Ready->SetText(FText::FromString(ReadyText));
}
