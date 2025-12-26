// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"


UENUM(BlueprintType)
enum class EEquipmentType :uint8
{
	None,
	Hammer,
	Fishing,
	Block,
	Puzzle
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALKAID_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	//equip stat
	UPROPERTY(ReplicatedUsing = OnRep_EquipmentType)
	EEquipmentType EquipmentType = EEquipmentType::None;

	UFUNCTION()
	void OnRep_EquipmentType();

	//equip held items
	UPROPERTY(ReplicatedUsing = OnRep_HeldItems)
	TObjectPtr<AActor> HeldItemRight = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_HeldItems)
	TObjectPtr<AActor> HeldItemLeft = nullptr;

	UFUNCTION()
	void OnRep_HeldItems();

	//equip Item
	UFUNCTION(Server, Reliable)
	void ServerEquipRightItem(AActor* Item, EEquipmentType NewType);

	UFUNCTION(Server, Reliable)
	void ServerEquipLeftItem(AActor* Item, EEquipmentType NewType);

	UFUNCTION(Server, Reliable)
	void ServerUnequipAllItems();

	void ItemDrop(TObjectPtr<AActor>& ItemSlot);

	void RequestInteractToggle(AActor* CandidateItem);

	UFUNCTION(Server, Reliable)
	void ServerToggleInteract(AActor* CandidateItem);

	UFUNCTION(Server, Reliable)
	void ServerDropItem();

	FORCEINLINE EEquipmentType GetEquipmentType() const { return EquipmentType; }
	FORCEINLINE AActor* GetHeldItemRight() const { return HeldItemRight; }

	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void ApplyAttach();

	void ApplyHeldVisual(AActor* Item, bool bHeld);

	void DropActorToFront(AActor* Item);

	class AAlkaidCharacter* GetOwnerCharacter() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment|Class")
	TMap<TSubclassOf<AActor>, EEquipmentType> ItemClassToEquipmentType;

	EEquipmentType DetermineEquipmentType(AActor* Item) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment|Attach")
	FName LeftHandSocket = TEXT("HandleSocket_l");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment|Attach")
	FName RightHandSocket = TEXT("HandleSocket_r");

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
