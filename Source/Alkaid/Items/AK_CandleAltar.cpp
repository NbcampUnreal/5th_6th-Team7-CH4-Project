// Fill out your copyright notice in the Description page of Project Settings.


#include "AK_CandleAltar.h"
#include "Alkaid/Character/AlkaidCharacter.h"
#include "Alkaid/Items/AKComponents/BuffComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

AAK_CandleAltar::AAK_CandleAltar()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true; // 시각적 피드백을 위해 Tick 활성화
	
	PickupMesh->SetRenderCustomDepth(bHasCharactersInRange);
}

void AAK_CandleAltar::BeginPlay()
{
	Super::BeginPlay();

	//서버에서만 콜백 바인딩
	if (HasAuthority())
	{
		// 범위 진입 이벤트 바인딩
		if (OverlapSphere)
		{
			// 부모쪽에서 이미 OnSphereOverlap을 바인드함
			//OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AHealthPickup::OnSphereOverlap);

			// 범위 벗어남 이벤트 바인딩 (새로 추가)
			OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &AAK_CandleAltar::OnSphereEndOverlap);
		}
		if (PickupMesh)
		{
			ReplicatedRotation = PickupMesh->GetComponentRotation();
		}
	}
	// 초기 시각적 피드백 설정
	UpdateVisualFeedback();
}

void AAK_CandleAltar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 매 프레임마다 시각적 피드백 갱신
	if (bEnableVisualFeedback)
	{
		// 클라이언트에서 직접 호출용
		//UpdateVisualFeedback();

		// 메쉬 회전 처리
		static bool bLastState = false;
		bool bCurrentState = CharactersInRange.Num() > 0;
		if (HasAuthority())
		{
			if (bCurrentState != bLastState)
			{
				const float RotationThisFrame = 180.0f * DeltaTime;
				ReplicatedRotation = ReplicatedRotation + FRotator(0.f, RotationThisFrame, 0.f);
				if (PickupMesh)
				{
					PickupMesh->SetWorldRotation(ReplicatedRotation);
				}
			}
		}
		else
		{
			// 클라이언트 : 복제된 값 적용
			if (PickupMesh)
			{
				PickupMesh->SetWorldRotation(ReplicatedRotation);
			}
		}

		// 메쉬 강조만 표현 -> 서버에서 OnRep_HasCharactersInRange()로 대체 
		//MulticastUpdateVisualFeedback(bCurrentState);
	}
}

void AAK_CandleAltar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// ReplicatedRotation 자동 동기화
	DOREPLIFETIME(AAK_CandleAltar, ReplicatedRotation);
	// 범위 안 캐릭터에게 PickupMesh 커스텀 뎁스 변화
	DOREPLIFETIME(AAK_CandleAltar, bHasCharactersInRange);
}

void AAK_CandleAltar::OnRep_HasCharactersInRange()
{
	if (PickupMesh)
	{
		PickupMesh->SetRenderCustomDepth(bHasCharactersInRange);
	}
}


void AAK_CandleAltar::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*
		이 함수는 서버의 콜리전 컴포넌트에서만 호출됨
		클라이언트의 콜리전 컴포넌트는 호출하지 않음
		(또는 호출해도 클라이언트는 게임 로직을 실행할 수 없음)
	*/
	// 1단계: null 체크
	if (!OtherActor)
	{
		return;
	}

	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	// 캐릭터 캐스팅 - 캐릭터가 없거나 죽은 캐릭터는 처리하지 않는다.
	AAlkaidCharacter* AKCharacter = Cast<AAlkaidCharacter>(OtherActor);
	if (!AKCharacter || AKCharacter->IsElimmed())
	{
		return;
	}

	// 범위 내 캐릭터 목록에 추가
	// AddUnique는 배열내 추가하려는 요소의 중복 여부 확인
	bool bWasEmpty = CharactersInRange.Num() == 0;
	CharactersInRange.AddUnique(AKCharacter);	

	//  이 값 변경이 자동으로 클라이언트에 동기화되고
   //    OnRep_HasCharactersInRange() 호출!
	if (bWasEmpty && CharactersInRange.Num() > 0 && HasAuthority())
	{
		bHasCharactersInRange = true;
	}

	if (AKCharacter)
	{
		UBuffComponent* Buff = AKCharacter->GetBuffComp();
		if (Buff)
		{
			Buff->Heal(HealAmount, HealingTime);	// 제단 안에 진입하면 회복 시작
		}
		else
		{
			CharactersInRange.Remove(AKCharacter);
			return;
		}

		// 이전 타이머가 있으면 제거
		if (ReHealTimerHandles.Contains(AKCharacter))
		{
			FTimerHandle* TimerHandle = ReHealTimerHandles.Find(AKCharacter);
			if (TimerHandle && GetWorld())
			{
				GetWorld()->GetTimerManager().ClearTimer(*TimerHandle);
			}
			ReHealTimerHandles.Remove(AKCharacter);
		}

		// 수정: RPC를 통해 모든 클라이언트에 이펙트 명령 전송
		MulticastPlayPickupEffect();

		// 회복 효과 재생 -> 스탠드 얼론 전용
		//if (PickupEffect)
		//{
		//	// 이것은 Niagara 파티클 이펙트
		//	// 화면에 표시해야 하는 렌더링 작업
		//	// 서버는 화면이 없으므로 의미 없음!
		//	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		//		this,
		//		PickupEffect,
		//		GetActorLocation(),
		//		GetActorRotation()
		//	);
		//}
		UE_LOG(LogTemp, Warning, TEXT("[HealthPickup] 플레이어 진입: %s | 현재 범위 내 플레이어: %d"),
			*AKCharacter->GetName(), CharactersInRange.Num());
	}
}

void AAK_CandleAltar::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AAlkaidCharacter* AKCharacter = Cast<AAlkaidCharacter>(OtherActor);
	if (AKCharacter && CharactersInRange.Contains(AKCharacter))
	{
		// 범위 내 목록에서 제거 
		CharactersInRange.Remove(AKCharacter);

		//  이 값 변경이 자동으로 클라이언트에 동기화되고
		// OnRep_HasCharactersInRange() 호출!
		if (CharactersInRange.Num() == 0 && HasAuthority())
		{
			bHasCharactersInRange = false;
		}

		// 회복 중지를 위해 BuffComponent에 신호 보냄
		UBuffComponent* Buff = AKCharacter->GetBuffComp();
		if (Buff)
		{
			// 현재 진행 중인 회복 중단
			Buff->StopHealing();
		}

		// 해당 플레이어의 활성화 타이머 제거
		if (ReHealTimerHandles.Contains(AKCharacter))
		{
			FTimerHandle* TimerHandle = ReHealTimerHandles.Find(AKCharacter);
			if (TimerHandle && GetWorld())
			{
				GetWorld()->GetTimerManager().ClearTimer(*TimerHandle);
			}
			ReHealTimerHandles.Remove(AKCharacter);
		}
		UE_LOG(LogTemp, Warning, TEXT("[HealthPickup] 플레이어 벗어남: %s | 현재 범위 내 플레이어: %d"),
			*AKCharacter->GetName(), CharactersInRange.Num());
	}
	else
	{
		return;
	}
}

void AAK_CandleAltar::ReactivateHealing(AAlkaidCharacter* Character)
{
	if (!Character || !IsCharacterInRange(Character))
	{
		return;
	}

	// 범위 내에 캐릭터가 여전히 있으면 다시 회복 시작
	if (!Character->IsElimmed())
	{
		UBuffComponent* Buff = Character->GetBuffComp();
		if (Buff)
		{
			Buff->Heal(HealAmount, HealingTime);

			UE_LOG(LogTemp, Log, TEXT("[HealthPickup] 재회복 시작: %s"), *Character->GetName());
		}
	}

	// 타이머 제거
	if (ReHealTimerHandles.Contains(Character))
	{
		ReHealTimerHandles.Remove(Character);
	}
}

void AAK_CandleAltar::UpdateVisualFeedback()
{
	// 서버에서 실행해도 렌더링 안 됨
   // 클라이언트에서만 보여야 함

	// 범위 내에 플레이어가 있는지 확인
	bool bHasCharacterInRange = CharactersInRange.Num() > 0;

	if (PickupMesh)
	{
		if (bHasCharacterInRange)
		{
			// 플레이어가 범위 내에 있으면 메쉬 강조
			PickupMesh->SetRenderCustomDepth(true);

			// 회전 효과 추가 (선택사항)
			FRotator CurrentRotation = PickupMesh->GetComponentRotation();
			PickupMesh->SetWorldRotation(CurrentRotation + FRotator(0.f, 10.f, 0.f));
		}
		else
		{
			// 플레이어가 범위 밖에 있으면 정상 상태로 복구
			PickupMesh->SetRenderCustomDepth(false);
		}
	}
}


bool AAK_CandleAltar::IsCharacterInRange(AAlkaidCharacter* Character) const
{
	return CharactersInRange.Contains(Character);
}

void AAK_CandleAltar::UpdateAllCharactersInRange()
{
	// 범위 내 모든 유효한 캐릭터 확인 (죽은 캐릭터 제거 )
	for (int32 i = CharactersInRange.Num() - 1; i >= 0; --i)
	{
		if (!CharactersInRange[i] || CharactersInRange[i]->IsElimmed())
		{
			CharactersInRange.RemoveAt(i);
		}
	}
}

void AAK_CandleAltar::OnCharacterElimmed(AActor* ElimedCharacter)
{
	AAlkaidCharacter* AKCharacter = Cast<AAlkaidCharacter>(ElimedCharacter);
	if (AKCharacter)
	{
		// 죽은 캐릭터가 범위 내에 있으면 제거
		if (CharactersInRange.Contains(AKCharacter))
		{
			CharactersInRange.Remove(AKCharacter);

			// 타이머 정리
			if (ReHealTimerHandles.Contains(AKCharacter))
			{
				FTimerHandle* TimerHandle = ReHealTimerHandles.Find(AKCharacter);
				if (TimerHandle && GetWorld())
				{
					GetWorld()->GetTimerManager().ClearTimer(*TimerHandle);
				}
				ReHealTimerHandles.Remove(AKCharacter);
			}

			UE_LOG(LogTemp, Warning, TEXT("[HealthPickup] 플레이어 사망: %s"), *AKCharacter->GetName());
		}
	}
	else
	{
		return;
	}
}

// ========================================
//  NetMulticast RPC 함수 구현
// ========================================
void AAK_CandleAltar::MulticastPlayPickupEffect_Implementation()
{
	//  모든 클라이언트(및 서버)에서 실행됨
   // 서버: 로컬 이펙트 재생
   // 클라이언트: 네트워크로부터 받은 명령으로 이펙트 재생
	if (PickupEffect)
	{
		//이것은 Niagara 파티클 이펙트
		// 화면에 표시해야 하는 렌더링 작업
		// 서버는 화면이 없으므로 의미 없음!
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			PickupEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}
}

void AAK_CandleAltar::MulticastUpdateVisualFeedback_Implementation(bool bPlayerInRange)
{
	//  모든 클라이언트에서 시각적 피드백 업데이트
   // 서버: 로컬 UI 업데이트
   // 클라이언트: 네트워크로부터 받은 상태로 UI 업데이트
	if (PickupMesh)
	{
		if (bPlayerInRange)
		{
			// 플레이어가 범위 내에 있으면 메쉬 강조
			PickupMesh->SetRenderCustomDepth(true);

			// 회전 효과 추가 
			//FRotator CurrentRotation = PickupMesh->GetComponentRotation();
			//PickupMesh->SetWorldRotation(CurrentRotation + FRotator(0.f, 10.f, 0.f));

			//UE_LOG(LogTemp, Warning, TEXT("[HealthPickup] 메쉬 강조: ON"));
		}
		else
		{
			// 플레이어가 범위 밖에 있으면 정상 상태로 복구
			PickupMesh->SetRenderCustomDepth(false);
		}
	}
}


