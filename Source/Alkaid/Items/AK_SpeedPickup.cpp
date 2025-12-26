// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/AK_SpeedPickup.h"
#include "Character/AlkaidCharacter.h"
#include "Items/AKComponents/BuffComponent.h"

void AAK_SpeedPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	AAlkaidCharacter* AKCharacter = Cast<AAlkaidCharacter>(OtherActor);
	if (AKCharacter)
	{
		// 탄약습득 시도 ( true 일경우 PickupAmmo 와 Destroy 함수 실행 )
		UBuffComponent* Buff = AKCharacter->GetBuffComp();
		if (Buff)
		{
			Buff->BuffSpeed(BaseSpeedBuff, CrouchSpeedBuff, SpeedBuffTime);
		}
	}
	Destroy();
}
