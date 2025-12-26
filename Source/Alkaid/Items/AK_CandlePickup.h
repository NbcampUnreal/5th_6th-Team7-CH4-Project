// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/AK_Pickup.h"
#include "AK_CandlePickup.generated.h"

/**
 * 
 */
UCLASS()
class ALKAID_API AAK_CandlePickup : public AAK_Pickup
{
	GENERATED_BODY()
	
protected:
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	)override;

private:
	// 한번 습득시 양초 개수
	UPROPERTY(EditAnywhere)
	int32 CandleAmount = 1;

};
