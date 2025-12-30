#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gamemode/PuzzleTypes.h"
#include "PuzzlePressObject.generated.h"

class UStaticMeshComponent;

UCLASS()
class ALKAID_API APuzzlePressObject : public AActor
{
    GENERATED_BODY()

public:
    APuzzlePressObject();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Puzzle")
    FPuzzleId OwnerPuzzleId; 
};