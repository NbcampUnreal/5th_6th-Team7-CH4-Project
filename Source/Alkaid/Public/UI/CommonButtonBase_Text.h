#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "CommonButtonBase_Text.generated.h"

class UCommonTextBlock;

UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class ALKAID_API UCommonButtonBase_Text : public UCommonButtonBase
{
	GENERATED_BODY()

	public:
		UFUNCTION(BlueprintCallable)
		void SetButtonText(FText InText);

	private:
		virtual void NativePreConstruct() override;

		virtual void NativeOnCurrentTextStyleChanged() override;
		
		UPROPERTY(meta = (BindWidgetOptional))
		UCommonTextBlock* CommonTextBlock_ButtonText;
		

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontend Button", meta = (AllowPrivateAccess = "true"))
		FText ButtonDisplayText;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontend Button", meta = (AllowPrivateAccess = "true"))
		bool bUserUpperCaseForButtonText = false;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontend Button", meta = (AllowPrivateAccess = "true"))
		FText ButtonDescriptionText;
	};
