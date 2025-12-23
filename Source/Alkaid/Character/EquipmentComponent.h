// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"


/*UENUM(BluePrintType)
enum class EEquipmentType :uint16
{
	None,
	Wepon,
	Block,
	Puzzle
};
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALKAID_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//UPROPERTY(ReplicatedUsing = OnRep_EquipmentType)
	//EEquipmentType EqupmentType = EEquipmentType::None;



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
