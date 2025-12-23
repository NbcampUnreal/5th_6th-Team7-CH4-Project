#include "HUD/StaminaWidget_UI.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"


void UStaminaWidget_UI::NativeConstruct()
{
    Super::NativeConstruct();

    CurrentPercent = 1.0f;
    MaxPercent = 1.0f;

    SetRenderOpacity(0.0f); //시작하자마 투명도를 0으로 설정하여 안보이게끔

    if (StaminaBarM)
    {
        //머터리얼 인스턴스를 동적 머터리얼로 변환
        StaminaDynamicMaterial = StaminaBarM->GetDynamicMaterial();
    }
}

void UStaminaWidget_UI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    //스테미나 퍼센트 보간 스무스하게 움직이게끔
    CurrentPercent = FMath::FInterpTo(CurrentPercent, MaxPercent, InDeltaTime, InterpSpeed);

    if (StaminaDynamicMaterial)
    {
        StaminaDynamicMaterial->SetScalarParameterValue(FName("Percent"), CurrentPercent);
    }

    //UI 100되면 숨김 
    float TargetOpacity = (MaxPercent >= 0.99f) ? 0.0f : 1.0f;

    float CurrentOpacity = GetRenderOpacity();

    if (!FMath::IsNearlyEqual(CurrentOpacity, TargetOpacity, 0.001f))
    {
        float NewOpacity = FMath::FInterpTo(CurrentOpacity, TargetOpacity, InDeltaTime, FadeUI);
        SetRenderOpacity(NewOpacity);
    }
}

void UStaminaWidget_UI::UpdateStaminaBar(float CurrentStamina, float MaxStamina)
{

    if (MaxStamina > 0.0f)
    {
        MaxPercent = CurrentStamina / MaxStamina;
    }
}