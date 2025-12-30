#include "Gamemode/PuzzlePressObject.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

APuzzlePressObject::APuzzlePressObject()
{
    PrimaryActorTick.bCanEverTick = false;
    SetReplicateMovement(true);
    bReplicates = true;
    NetUpdateFrequency = 60.f;
    MinNetUpdateFrequency = 30.f;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);

    Mesh->SetIsReplicated(true);
    Mesh->SetGenerateOverlapEvents(true);

    Mesh->SetSimulatePhysics(true);
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    Mesh->SetCollisionObjectType(ECC_PhysicsBody);
}