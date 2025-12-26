// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/AKComponents/BuffComponent.h"
#include "Alkaid/Character/AlkaidCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Alkaid/Character/AlkaidCharaterStatComponent.h"

UBuffComponent::UBuffComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

	
}


// Called when the game starts
void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();

	// Character 포인터 초기화 
	if (!AKCharacter)
	{
		AKCharacter = Cast<AAlkaidCharacter>(GetOwner());
	}

	// 로그 출력 (디버깅용)
	if (AKCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BuffComponent] 초기화 완료 | Owner: %s"), *AKCharacter->GetName());
	}

	// AKStatComp 자동 탐색
	if (!AKStatComp && AKCharacter)
	{
		AKStatComp = AKCharacter->FindComponentByClass<UAlkaidCharaterStatComponent>();
	}
	// 디버그 로그
	if (AKCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BuffComponent] 초기화 완료 | Owner: %s | StatComp: %s"),
			*AKCharacter->GetName(),
			AKStatComp ? TEXT("Valid") : TEXT("NULL"));
	}
}

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 서버에서만 회복 로직 실행 (클라이언트는 복제를 통해 동기화)
	if (GetOwnerRole() == ROLE_Authority)
	{
		HealRampUp(DeltaTime);
	}
}

void UBuffComponent::Heal(float HealAmount, float HealingTime)
{
	// 서버에서만 회복 시작
	if (GetOwnerRole() != ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BuffComponent] 서버가 아님 - 회복 무시"));
		return;
	}

	// 추가: 안전 검사
	if (!AKCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[Heal] AKCharacter is nullptr!"));
		return;
	}

	if (!AKStatComp)
	{
		UE_LOG(LogTemp, Error, TEXT("[Heal] AKStatComp is nullptr!"));
		return;
	}

	// 회복 시간이 0이면 즉시 체력 회복
	if (HealingTime <= 0.f)
	{
		if (AKCharacter)
		{
			AKStatComp->SetStamina(FMath::Clamp(AKStatComp->GetStamina() + HealAmount, 0.f, AKStatComp->GetMaxStamina()));
			AKStatComp->UpdateHUDStamina();
		}
		return;
	}

	// 시간 경과 회복 설정
	bHealing = true;
	HealingRate = HealAmount / HealingTime;
	AmountToHeal += HealAmount;

	UE_LOG(LogTemp, Warning, TEXT("[BuffComponent] 회복 시작 | 회복량: %.1f | 시간: %.1f초 | 회복속도: %.2f/s"), HealAmount, HealingTime, HealingRate);
}

void UBuffComponent::StopHealing()
{
	// 회복 중단 - 현재 진행 중인 회복을 멈추고 초기화
	if (bHealing)
	{
		bHealing = false;
		AmountToHeal = 0.f;
		HealingRate = 0.f;

		UE_LOG(LogTemp, Warning, TEXT("[BuffComponent] 회복 중단"));
	}
}


void UBuffComponent::HealRampUp(float DeltaTime)
{
	// 회복하지 않는 상태나 캐릭터가 범위를 벗어나면 조기 종료
	if (!bHealing || AKCharacter == nullptr || AKCharacter->IsElimmed()) return;

	// 추가: AKStatComp가 nullptr일 때 처리
	if (!AKStatComp)
	{
		UE_LOG(LogTemp, Error, TEXT("[HealRampUp] AKStatComp 가 Null! 회복을 중단!"));
		bHealing = false;
		AmountToHeal = 0.f;
		return;  // ← crash 방지
	}

	// 이번 프레임에 회복할 양 계산
	const float HealThisFrame = HealingRate * DeltaTime;

	// 현재 체력에 회복량을 더하고 유효 범위로 제한
	float NewHealth = AKStatComp->GetStamina() + HealThisFrame;
	NewHealth = FMath::Clamp(NewHealth, 0.f, AKStatComp->GetMaxStamina());
	AKStatComp->SetStamina(NewHealth);

	// HUD 업데이트
	AKStatComp->UpdateHUDStamina();

	// 남은 회복량 차감
	AmountToHeal -= HealThisFrame;

	// 회복이 끝났거나 체력이 가득 찼는지 확인
	if (AmountToHeal <= 0.f || AKStatComp->GetStamina() >= AKStatComp->GetMaxStamina())
	{
		// 회복 종료
		bHealing = false;
		AmountToHeal = 0.f;

		UE_LOG(LogTemp, Warning, TEXT("[BuffComponent] 회복 완료 | 최종 체력: %.1f / %.1f"),
			AKStatComp->GetStamina(), AKStatComp->GetMaxStamina());
	}
}


void UBuffComponent::SetInitialSpeeds(float BaseSpeed, float CrouchSpeed)
{
	InitialBaseSpeed = BaseSpeed;
	InitialCrouchSpeed = CrouchSpeed;
}

void UBuffComponent::BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime)
{
	if (AKCharacter == nullptr) return;

	AKCharacter->GetWorldTimerManager().SetTimer(
		SpeedBuffTiemr,
		this,
		&UBuffComponent::ResetSpeeds,
		BuffTime
	);

	if (AKCharacter->GetCharacterMovement())	// CharacterMovement 초기화
	{
		AKCharacter->GetCharacterMovement()->MaxWalkSpeed = BuffBaseSpeed;
		AKCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = BuffCrouchSpeed;
	}
	MulticastSpeedBuff(BuffBaseSpeed, BuffCrouchSpeed);
}



void UBuffComponent::ResetSpeeds()
{
	if (AKCharacter == nullptr || AKCharacter->GetCharacterMovement() == nullptr) return;

	AKCharacter->GetCharacterMovement()->MaxWalkSpeed = InitialBaseSpeed;
	AKCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = InitialCrouchSpeed;
	MulticastSpeedBuff(InitialBaseSpeed, InitialCrouchSpeed);
}

void UBuffComponent::MulticastSpeedBuff_Implementation(float BaseSpeed, float CrouchSpeed)
{
	AKCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
	AKCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
}



