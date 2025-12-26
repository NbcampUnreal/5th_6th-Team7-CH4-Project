#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gamemode/PuzzleTypes.h"
#include "PuzzleColorButton.generated.h"

class UBoxComponent;
class UCapsuleComponent;
class UShapeComponent;
class UPrimitiveComponent;
class UStaticMeshComponent;
class USceneComponent; // 수정/추가: forward declare 추가 (Root에 사용)

// 버튼 상태가 바뀌면 서버에 알림
// Button은 어떤 버튼인지
// bPressed는 현재 눌림상태 판단
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPuzzleButtonPressedChanged, APuzzleColorButton*, Button, bool, bPressed);

UCLASS()
class ALKAID_API APuzzleColorButton : public AActor
{
	GENERATED_BODY()

public:
	APuzzleColorButton();

	// 현재 눌림 상태를 반환합니다
	// 클라이언트는 이 값을 보기만 합니다
	UFUNCTION(BlueprintCallable, Category = "Puzzle|Button")
	bool IsPressed() const { return bPressed; }

	// 버튼 식별자 ex: 1_1_RedButton
	// 레벨 인스턴스마다 입력
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Puzzle|Button")
	FPuzzleId ButtonId;

	// 눌림 상태가 바뀌면 호출
	// 게임모드가 서버에서 바인딩
	UPROPERTY(BlueprintAssignable, Category = "Puzzle|Button")
	FOnPuzzleButtonPressedChanged OnPressedChanged;

	// 눌림 연출을 블루프린트에서 처리하고 싶을 때 사용. 안쓰지만 혹시몰라서 구현

	UFUNCTION(BlueprintImplementableEvent, Category = "Puzzle|Button")
	void BP_OnPressedVisualChanged(bool bNowPressed);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root = nullptr;

	// 버튼 외형 메시
	// 메쉬에 충돌은 불필요하므로, 꺼둠
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Visual = nullptr;

	// 버튼 위를 감지하는 트리거
	// 오버랩으로 눌림을 판정
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UShapeComponent> Trigger = nullptr; // (Box/Capsule 공용)

	// Box 트리거를 쓸 때만 사용 (BP에서 둘 다 붙여놓고 선택 가능)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerBox = nullptr; // Box 트리거 참조용

	// Capsule 트리거를 쓸 때만 사용 (BP에서 둘 다 붙여놓고 선택 가능)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCapsuleComponent> TriggerCapsule = nullptr; // Capsule 트리거 참조용

	// PuzzleTypes.h에 만든 enum 사용
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puzzle")
	EPuzzleTriggerShape TriggerShape = EPuzzleTriggerShape::Box;

	// 눌림 상태
	// 서버가 값을 바꾸고 클라이언트에 복제
	UPROPERTY(ReplicatedUsing = OnRep_Pressed, VisibleAnywhere, BlueprintReadOnly, Category = "Puzzle|Button")
	bool bPressed = false;

	// 특정 태그만 눌림으로 인정하고 싶을 때 씀(2-3에서, 버튼에 물체를 올려 작동하게하는것을 방지)
	// 비워두면 태그 체크 x
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puzzle|Button|Filter")
	FName RequiredActorTag = NAME_None;

protected:
	virtual void BeginPlay() override;

	// bPressed가 복제되어 바뀌면 호출
	UFUNCTION()
	void OnRep_Pressed();

	// 트리거에 무언가 들어오면 호출
	// 서버에서만 눌림 카운트를 올림
	UFUNCTION()
	void OnTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	// 트리거에서 무언가 나가면 호출
	// 서버에서만 눌림 카운트를 내림
	UFUNCTION()
	void OnTriggerEndOverlap(
		UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 눌림 인정 대상 검사
	bool IsValidActivator(AActor* OtherActor) const;

	// 서버에서 눌림 상태를 변경
	// 상태변환 시 델리게이트를 방송
	void ServerSetPressed(bool bNewPressed);

	// 선택된 트리거를 찾아서 설정
	// Box 또는 Capsule 중 하나를 Trigger로 지정
	void ResolveTriggerComponent(); // Box/Capsule 자동 선택을 위한 함수 선언

public:
	// 네트워크 복제를 위해 필요한 함수
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// 서버에서만 쓰는 카운트
	// 여러 개가 동시에 올라가도 눌림을 유지하기 위함
	int32 OverlapCount = 0;
};