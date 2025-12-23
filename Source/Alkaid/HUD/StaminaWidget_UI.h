#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StaminaWidget_UI.generated.h"

class UImage;
class UMaterialInstanceDynamic;

UCLASS()
class ALKAID_API UStaminaWidget_UI : public UUserWidget
{
	GENERATED_BODY()

public:

    UPROPERTY(meta = (BindWidget))
    UImage* StaminaBarM;

    UPROPERTY()
    UMaterialInstanceDynamic* StaminaDynamicMaterial;

    virtual void NativeConstruct() override;

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Stamina")
    void UpdateStaminaBar(float CurrentStamina, float MaxStamina);

    //보간처리
    float CurrentPercent;
    float MaxPercent;

    UPROPERTY(EditAnywhere, Category = "Stamina")
    float FadeUI = 3.0f; //UI 숨김

    UPROPERTY(EditAnywhere, Category = "Stamina")
    float InterpSpeed = 5.0f; //숫자가 높을수록 빠름
	
};
