#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Sound/SoundBase.h"

#include "CommonUserWidget_GameMainUI.generated.h"


UCLASS()
class ALKAID_API UCommonUserWidget_GameMainUI : public UCommonUserWidget
{
	GENERATED_BODY()

public:


    //Yes위젯
	UPROPERTY(meta = (BindWidget))
	class USizeBox* SizeBox_Yes;

    UPROPERTY(meta = (BindWidget))
    class UButton* Button_Yes;

    UPROPERTY(meta = (BindWidget))
    class UBorder* Border_Yes;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* CommonTextBlock_Yes;


    //No위젯
    UPROPERTY(meta = (BindWidget))
	class USizeBox* SizeBox_NO;

    UPROPERTY(meta = (BindWidget))
    class UButton* Button_NO;

    UPROPERTY(meta = (BindWidget))
    class UBorder* Border_NO;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* CommonTextBlock_NO;

    //사운드 설정
    UPROPERTY(EditAnywhere, Category = "UISound")
    USoundBase* ClickSound;

    //내부 로직
    virtual void NativeConstruct() override;

    //Yes버튼
    UFUNCTION()
    void OnButtonYesHovered();
    UFUNCTION()
    void OnButtonYesUnhovered();
    UFUNCTION() 
    void OnButtonYesClicked();

    //No버튼
    UFUNCTION() 
    void OnButtonNoHovered();
    UFUNCTION() 
    void OnButtonNoUnhovered();
    UFUNCTION() 
    void OnButtonNoClicked();
	
};
