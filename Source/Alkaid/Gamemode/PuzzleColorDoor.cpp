#include "Gamemode/PuzzleColorDoor.h"
#include "Components/StaticMeshComponent.h"

APuzzleColorDoor::APuzzleColorDoor()
{
	// 틱 체크로 안함 (메모리 사용줄일 목적)
	PrimaryActorTick.bCanEverTick = false;

	// 멀티에서 네트워크로 복제
	bReplicates = true;

	// 문 외형 메시 컴포넌트를 만듭니다
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	// 기본적인 콜리전 설정입니다
	// 블루프린트에서 변경가능
	Mesh->SetCollisionProfileName(TEXT("BlockAll"));

	// 메쉬에 오버랩 이벤트가 불필요
	Mesh->SetGenerateOverlapEvents(false);
}

void APuzzleColorDoor::OpenDoor()
{
	// 문 제거 판단은 서버 담당

	if (!HasAuthority())
	{
		return;
	}

	// 문열림 = 제거처리
	// 서버에서 Destroy하면 클라이언트에도 반영. 추후 변경가능
	Destroy();
}