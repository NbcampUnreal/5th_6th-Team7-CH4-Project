// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/AlkaidCharaterStatComponent.h"
#include "Character/EquipmentComponent.h"
#include "InputActionValue.h"
#include "AlkaidCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UUserWidget;

UCLASS()
class ALKAID_API AAlkaidCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAlkaidCharacter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Camera

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AlkaidCharacter|Component")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AlkaidCharacter|Component")
	TObjectPtr<UCameraComponent> Camera;

	//component

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AlkaidCharacter|Component")
	UAlkaidCharaterStatComponent* StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AlkaidCharacter|Component")
	UEquipmentComponent* EquipmentComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AlkaidCharacter|Component")
	class UItemComponent* ItemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AlkaidCharacter|Component")
	class UBuffComponent* BuffComponent;

	//component
	void PostInitializeComponents() override;

public:
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }

	//Sprint
	UPROPERTY(ReplicatedUsing = OnRep_IsSprinting)
	bool bIsSprinting = false;

	UFUNCTION()
	void OnRep_IsSprinting();

	void SprintSpeed_Server();

	FORCEINLINE bool IsSprinting() const { return bIsSprinting; }

	//server
	UFUNCTION(Server, Reliable)
	void ServerUseCandle(); 

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void ServerSetSprinting(bool NewSprinting);

	// Pushing

	UPROPERTY(ReplicatedUsing = OnRep_Pushing)
	TObjectPtr<AActor> Pushing = nullptr;

	UFUNCTION()
	void OnRep_Pushing();

	UFUNCTION(Server, Reliable)
	void ServerStartPushing(AActor* NewBlock);

	UFUNCTION(Server, Reliable)
	void ServerStopPushing();

	FORCEINLINE bool bIsPushing() const { return Pushing != nullptr; }

	FORCEINLINE AActor* GetPushing() const { return Pushing; }

	//Input
private:
	void HandleMoveInput(const FInputActionValue& InValue);

	void HandleLookInput(const FInputActionValue& InValue);
	
	void HandleUsingCandleInput(const FInputActionValue& InValue);

	void HandleAttackInput(const FInputActionValue& InValue);

	void HandleEscUIInput(const FInputActionValue& InValue);

	void HandleReadyInput(const FInputActionValue& InValue);

	void HandleStartInput(const FInputActionValue& InValue);

	void StartSprint(const FInputActionValue& Invalue);

	void StopSprint(const FInputActionValue& Invalue);

	void UsingItemInputStarted(const FInputActionValue& Invalue);

	void UsingItemInputCompleted(const FInputActionValue& Invalue);

	void UsingItemInputCanceled(const FInputActionValue& Invalue);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AlkaidCharacter|Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AlkaidCharacter|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AlkaidCharacter|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AlkaidCharacter|Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AlkaidCharacter|Input")
	TObjectPtr<UInputAction> UsingItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AlkaidCharacter|Input")
	TObjectPtr<UInputAction> UsingCandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AlkaidCharacter|Input")
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AlkaidCharacter|Input")
	TObjectPtr<UInputAction> EscUI;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AlkaidCharacter|Input")
	TObjectPtr<UInputAction> ReadyAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AlkaidCharater|Input")
	TObjectPtr<UInputAction> StartAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AlkaidCharacter|Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> PuzzleClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> BlockClass;

	UPROPERTY(EditAnywhere, Category = "EscMessage")
	TSubclassOf<UUserWidget> EscWidgetClass;

	UPROPERTY(EditAnywhere, Category = "StaminaWidget")
	TSubclassOf<UUserWidget> StaminaWidget;

	UPROPERTY()
	TObjectPtr<AActor> CurrentInteractItem = nullptr;

private:

	UPROPERTY()
	TObjectPtr<UUserWidget> EscWidgetInstance;

	UPROPERTY()
	TObjectPtr<UUserWidget> StaminaWidgetInstance;

	UPROPERTY()
	bool bElimmed = false;	// 촛불 제단안에 들어왔는가 아닌가


public:
	// Getter, Setter
	FORCEINLINE UItemComponent* GetItemComp() const { return ItemComponent; }
	FORCEINLINE UBuffComponent* GetBuffComp() const { return BuffComponent; }
	// 촛불제단안에 들어왔는가 아닌가
	FORCEINLINE bool IsElimmed() const { return bElimmed; }		

};
