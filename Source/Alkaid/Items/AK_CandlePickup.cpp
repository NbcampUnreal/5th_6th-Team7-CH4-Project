// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/AK_CandlePickup.h"
#include "Character/AlkaidCharacter.h"
#include "Items/AKComponents/ItemComponent.h"

void AAK_CandlePickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	AAlkaidCharacter* AKCharacter = Cast<AAlkaidCharacter>(OtherActor);
	if (AKCharacter)
	{
		// 탄약습득 시도 ( true 일경우 PickupAmmo 와 Destroy 함수 실행 )
		UItemComponent* Item = AKCharacter->GetItemComp();
		bool bPickupSuccess = Item->PickupCandle(CandleAmount);
		if (bPickupSuccess)
		{
			Destroy();
		}
	}
	
	
}
