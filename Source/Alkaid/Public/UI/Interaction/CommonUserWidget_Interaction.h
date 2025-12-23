#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "CommonUserWidget_Interaction.generated.h"


UCLASS()
class ALKAID_API UCommonUserWidget_Interaction : public UCommonUserWidget
{
	GENERATED_BODY()

public:

	float RkeyAngle = 0.0f;
	float CurrentAngle = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Angle")
	float RotationAngleSpeed = 10.0f;

	UFUNCTION(BlueprintCallable)
	void InteractionRotation();

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
};
