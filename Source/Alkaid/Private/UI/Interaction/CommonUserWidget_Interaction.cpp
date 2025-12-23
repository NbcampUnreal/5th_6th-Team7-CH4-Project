#include "UI/Interaction/CommonUserWidget_Interaction.h"

void UCommonUserWidget_Interaction::InteractionRotation()
{
	RkeyAngle += 360.0f;
}

void UCommonUserWidget_Interaction::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

    if (!FMath::IsNearlyEqual(CurrentAngle, RkeyAngle, 0.1f)) //값A, 값B, 오차범위
    {
        CurrentAngle = FMath::FInterpTo(CurrentAngle, RkeyAngle, InDeltaTime, RotationAngleSpeed); //보간처리

        float CosValue = FMath::Cos(FMath::DegreesToRadians(CurrentAngle)); //라디안 단위로 변환

        SetRenderScale(FVector2D(CosValue, 1.0f)); //X축으로만 조절
    }
}
