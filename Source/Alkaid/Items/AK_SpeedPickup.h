// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/AK_Pickup.h"
#include "AK_SpeedPickup.generated.h"

/**
 * 
 */
UCLASS()
class ALKAID_API AAK_SpeedPickup : public AAK_Pickup
{
	GENERATED_BODY()

protected:

	// 범위에 진입했을 때 처리
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;

private:

	// 최대 버프 속도
	UPROPERTY(EditAnywhere)
	float BaseSpeedBuff = 1600.f;

	// 크라우치 버프 속도
	UPROPERTY(EditAnywhere)
	float CrouchSpeedBuff = 850.f;

	UPROPERTY(EditAnywhere)
	float SpeedBuffTime = 30.f;
};
