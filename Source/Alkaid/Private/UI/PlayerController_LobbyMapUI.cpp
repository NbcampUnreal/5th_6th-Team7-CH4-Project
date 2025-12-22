#include "UI/PlayerController_LobbyMapUI.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Blueprint/UserWidget.h"

#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

#include "Server/AlkaidGameModeBase.h"

void APlayerController_LobbyMapUI::RequestTravelToLobby()
{
    Server_TravelToLobby();

    if (LobbyWidgetInstance)
    {
        LobbyWidgetInstance->RemoveFromParent();
        LobbyWidgetInstance = nullptr;
    }

    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
    bShowMouseCursor = false;
}

void APlayerController_LobbyMapUI::Server_TravelToLobby_Implementation()
{
    if (!HasAuthority()) return;

    UWorld* World = GetWorld();
    if (World)
    {
        AAlkaidGameModeBase* MoveMap = Cast<AAlkaidGameModeBase>(World->GetAuthGameMode());
        if (MoveMap)
        {
            MoveMap->TravelTo(MoveMap->LobbyMapPath);
        }
    }
}

void APlayerController_LobbyMapUI::BeginPlay()
{
    Super::BeginPlay();

    bAutoManageActiveCameraTarget = false;
}

void APlayerController_LobbyMapUI::OnPossess(APawn* pawn)
{
    Super::OnPossess(pawn);

    Client_SetupLobby();
}

void APlayerController_LobbyMapUI::Client_SetupLobby_Implementation()
{
    FTimerHandle CamerTimer;
    GetWorldTimerManager().SetTimer(CamerTimer, this, &APlayerController_LobbyMapUI::LobbySettings, 0.1f, false);
}

void APlayerController_LobbyMapUI::LobbySettings()
{
    //Camera Find
    TArray<AActor*> FoundCameras;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), FoundCameras);

    for (AActor* Actor : FoundCameras)
    {
        if (Actor && Actor->ActorHasTag(FName("Default")))
        {
            SetViewTarget(Actor);
            break;
        }
    }
    //Widget
    if (LobbyWidgetClass && !LobbyWidgetInstance)
    {
        LobbyWidgetInstance = CreateWidget<UUserWidget>(this, LobbyWidgetClass);

        if (LobbyWidgetInstance)
        {
            LobbyWidgetInstance->AddToViewport();

            FInputModeUIOnly InputUI;

            InputUI.SetWidgetToFocus(LobbyWidgetInstance->TakeWidget());
            InputUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

            SetInputMode(InputUI);
            bShowMouseCursor = true;
        }
    }
    //Sound
    if (LobbySound)
    {
        UGameplayStatics::PlaySound2D(this, LobbySound);
    }
}