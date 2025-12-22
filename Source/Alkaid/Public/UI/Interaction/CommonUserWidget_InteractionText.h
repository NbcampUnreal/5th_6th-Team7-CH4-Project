#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "CommonUserWidget_InteractionText.generated.h"

class UTextBlock;

UCLASS()
class ALKAID_API UCommonUserWidget_InteractionText : public UCommonUserWidget
{
    GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
	void UpdatePlayerStates();

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextBlock_Ready;
};
