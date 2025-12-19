#include "UI/GameMainUI/CommonUserWidget_GameMainUI.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Math/Color.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"


void UCommonUserWidget_GameMainUI::NativeConstruct()
{
    Super::NativeConstruct();

    
    if (Button_Yes)
    {
        Button_Yes->OnHovered.AddDynamic(this, &UCommonUserWidget_GameMainUI::OnButtonYesHovered);
        Button_Yes->OnUnhovered.AddDynamic(this, &UCommonUserWidget_GameMainUI::OnButtonYesUnhovered);
        Button_Yes->OnClicked.AddDynamic(this, &UCommonUserWidget_GameMainUI::OnButtonYesClicked);
    }
    if (Button_NO)
    {
        Button_NO->OnHovered.AddDynamic(this, &UCommonUserWidget_GameMainUI::OnButtonNoHovered);
        Button_NO->OnUnhovered.AddDynamic(this, &UCommonUserWidget_GameMainUI::OnButtonNoUnhovered);
        Button_NO->OnClicked.AddDynamic(this, &UCommonUserWidget_GameMainUI::OnButtonNoClicked);
    }
}

void UCommonUserWidget_GameMainUI::OnButtonYesHovered()
{
    FLinearColor YesColor = FLinearColor(FColor::FromHex(TEXT("FFF59CFF")));

    if (Border_Yes)
    {
        Border_Yes->SetBrushColor(YesColor);
    }

    if (CommonTextBlock_Yes)
    {
        CommonTextBlock_Yes->SetColorAndOpacity(FSlateColor(YesColor));
    }
}

void UCommonUserWidget_GameMainUI::OnButtonYesUnhovered()
{
    if (Border_Yes)
    {
        Border_Yes->SetBrushColor(FLinearColor::White);
    }

    if (CommonTextBlock_Yes)
    {
        CommonTextBlock_Yes->SetColorAndOpacity(FSlateColor(FLinearColor::White));
    }
}

void UCommonUserWidget_GameMainUI::OnButtonYesClicked()
{
    if (ClickSound)
    {
        UGameplayStatics::PlaySound2D(this, ClickSound);
    }
}

void UCommonUserWidget_GameMainUI::OnButtonNoHovered()
{
    FLinearColor NoColor = FLinearColor(FColor::FromHex(TEXT("FFF59CFF")));

    if (Border_NO)
    {
        Border_NO->SetBrushColor(NoColor);
    }

    if (CommonTextBlock_NO)
    {
        CommonTextBlock_NO->SetColorAndOpacity(FSlateColor(NoColor));
    }
}

void UCommonUserWidget_GameMainUI::OnButtonNoUnhovered()
{
    if (Border_NO)
    {
        Border_NO->SetBrushColor(FLinearColor::White);
    }

    if (CommonTextBlock_NO)
    {
        CommonTextBlock_NO->SetColorAndOpacity(FSlateColor(FLinearColor::White));
    }
}

void UCommonUserWidget_GameMainUI::OnButtonNoClicked()
{
    if (ClickSound)
    {
        UGameplayStatics::PlaySound2D(this, ClickSound);
    }
}