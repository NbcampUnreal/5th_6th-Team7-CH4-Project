#include "Gamemode/PuzzlePressObject.h"
#include "Components/StaticMeshComponent.h"

APuzzlePressObject::APuzzlePressObject()
{
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);

    Mesh->SetSimulatePhysics(true);
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    Mesh->SetCollisionObjectType(ECC_PhysicsBody);
}