// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AlkaidCharacter.h"
#include "Controller/AlkaidPlayerController.h"
#include "Character/AlkaidCharaterStatComponent.h"
#include "Character/EquipmentComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Components/PrimitiveComponent.h"
#include "Items/AKComponents/ItemComponent.h"
#include "Items/AKComponents/BuffComponent.h"

/*static FString RoleToStr(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None: return TEXT("None");
	case ROLE_SimulatedProxy: return TEXT("SimProxy");
	case ROLE_AutonomousProxy: return TEXT("AutoProxy");
	case ROLE_Authority: return TEXT("Authority");
	default: return TEXT("Unknown");
	}
}

static FString NetModeToStr(ENetMode NM)
{
	switch (NM)
	{
	case NM_Standalone: return TEXT("Standalone");
	case NM_DedicatedServer: return TEXT("DedicatedServer");
	case NM_ListenServer: return TEXT("ListenServer");
	case NM_Client: return TEXT("Client");
	default: return TEXT("Unknown");
	}
}

#define AK_LOG_CTX(Format, ...) \
do { \
	const UWorld* W__ = GetWorld(); \
	const ENetMode NM__ = W__ ? W__->GetNetMode() : NM_Standalone; \
	UE_LOG(LogTemp, Warning, TEXT("[%s] %s | Local=%d Auth=%d Role=%s Remote=%s Ctrl=%s Owner=%s | " Format), \
		*NetModeToStr(NM__), \
		*GetNameSafe(this), \
		IsLocallyControlled(), \
		HasAuthority(), \
		*RoleToStr(GetLocalRole()), \
		*RoleToStr(GetRemoteRole()), \
		*GetNameSafe(GetController()), \
		*GetNameSafe(GetOwner()), \
		##__VA_ARGS__); \
} while(0)

#define AK_LOG_SPEED(Tag) \
do { \
	const float WS__ = GetCharacterMovement() ? GetCharacterMovement()->MaxWalkSpeed : -1.f; \
	const float Vel__ = GetVelocity().Size2D(); \
	AK_LOG_CTX(TEXT("%s | bIsSprinting=%d MaxWalkSpeed=%.1f Vel2D=%.1f"), TEXT(Tag), bIsSprinting, WS__, Vel__); \
} while(0)*/

// Sets default values
AAlkaidCharacter::AAlkaidCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	StatComponent = CreateDefaultSubobject<UAlkaidCharaterStatComponent>(TEXT("StatComponent"));
	StatComponent->SetIsReplicated(true);
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
	EquipmentComponent->SetIsReplicated(true);

	// ItemComponent 생성및 초기화
	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));
	ItemComponent->SetIsReplicated(true);

	// BuffComponent 생성및 초기화
	BuffComponent = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	BuffComponent->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void AAlkaidCharacter::BeginPlay()
{
	Super::BeginPlay();



	if (IsLocallyControlled() == true)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		//checkf(IsValid(PC) == true, TEXT("PlayerController is invalid."));

		UEnhancedInputLocalPlayerSubsystem* EILPS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		//checkf(IsValid(EILPS) == true, TEXT("EnhancedInputLocalPlayerSubsystem is invalid."));

		EILPS->AddMappingContext(InputMappingContext, 0);
		//StaminaWidgetInstance->AddToViewport();
		if (IsValid(EscWidgetClass) == true)
		{
			EscWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), EscWidgetClass);

		}
	}
	
}


void AAlkaidCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(StatComponent)
	{
		StatComponent->ApplySpeed();
		StatComponent->ApplyStamina();
	}
	if (ItemComponent)
	{
		ItemComponent->AKCharacter = this;
	}
	if (BuffComponent)
	{
		BuffComponent->AKCharacter = this;
		BuffComponent->AKStatComp = StatComponent;
		BuffComponent->SetInitialSpeeds(
			GetCharacterMovement()->MaxWalkSpeed,
			GetCharacterMovement()->MaxWalkSpeedCrouched
		);
	}
}

void AAlkaidCharacter::ServerUseCandle_Implementation()
{
	if (!StatComponent && !ItemComponent)
		return; 
	
	if(ItemComponent->GetCandle() <= 0)
		return;

	if (StatComponent->IsCandleOnCooldown())
		return;

	StatComponent->StartCandleCooldown();
	ItemComponent->SpendRound();
	StatComponent->AddStamina(20.0f);

	/*UE_LOG(LogTemp, Warning, TEXT("[SERVER] After Use: Stamina=%f/%f Candle=%f EndTime=%f"),
		StatComponent->GetStamina(),
		StatComponent->GetMaxStamina(),
		StatComponent->GetCandleCount(),
		StatComponent->CandleCoolDownEndTime);*/
	
}

void AAlkaidCharacter::OnRep_IsSprinting()
{
	if (StatComponent)
	{
		StatComponent->ApplySpeed();
	}
}

void AAlkaidCharacter::SprintSpeed_Server()
{
	//AK_LOG_SPEED("SERVER SprintSpeed_Server ENTER");

	if (!HasAuthority())
		return;
	if (!StatComponent)
		return;

	const float NewSpeed = StatComponent->GetFinalMoveSpeed(bIsSprinting);
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;

	//AK_LOG_CTX(TEXT("SprintSpeed_Server applied: Old=%.1f New=%.1f bIsSprinting=%d"), OldSpeed, NewSpeed, bIsSprinting);
}

void AAlkaidCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAlkaidCharacter, bIsSprinting);
	DOREPLIFETIME(AAlkaidCharacter, Pushing);
}

void AAlkaidCharacter::ServerSetSprinting_Implementation(bool NewSprinting)
{
	if (bIsSprinting == NewSprinting)
		return;

	bIsSprinting = NewSprinting;

	StatComponent->ApplySpeed();

	//AK_LOG_SPEED("SERVER ServerSetSprinting changed flag");

	//SprintSpeed_Server();

	//AK_LOG_SPEED("SERVER ServerSetSprinting AFTER SprintSpeed_Server");
}

void AAlkaidCharacter::ApplyPushingMovementSetting(bool bEnable)
{
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (!MoveComp) return;

	if (bEnable)
	{
		MoveComp->bOrientRotationToMovement = false;
		MoveComp->MaxWalkSpeed = 500.f; // 푸시 속도(원하는 값)
	}
	else
	{
		MoveComp->bOrientRotationToMovement = true;
		if (StatComponent) StatComponent->ApplySpeed();
	}

	// 푸시 중 흔들림 방지: 컨트롤러 yaw로 캐릭터 회전시키지 않음
	bUseControllerRotationYaw = false;
}

void AAlkaidCharacter::HandleMoveInput(const FInputActionValue& InValue)
{
	if (!IsValid(Controller))
		return;

	FVector2D Move = InValue.Get<FVector2D>();

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator ControlYawRotation(0.f, ControlRotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::Y);

	if (bIsPushing())
	{
		Move.Y = 0.f;                      // 옆 이동 차단
		Move.X = FMath::Max(0.f, Move.X);  // 뒤 이동 차단

		SetActorRotation(FRotator(0.f, PushingYaw, 0.f)); // 방향 고정

		if (Move.X > KINDA_SMALL_NUMBER)
		{
			AddMovementInput(GetActorForwardVector(), Move.X);
		}
		return;
	}

	AddMovementInput(ForwardDirection, Move.X);
	AddMovementInput(RightDirection, Move.Y);
}


void AAlkaidCharacter::HandleLookInput(const FInputActionValue& InValue)
{
	if(IsValid(Controller) == false)
	{
		return;
	}

	const FVector2D InLookVector = InValue.Get<FVector2D>();

	AddControllerYawInput(InLookVector.X);
	AddControllerPitchInput(InLookVector.Y);
}

void AAlkaidCharacter::HandleUsingCandleInput(const FInputActionValue& InValue)
{
	
	/*UE_LOG(LogTemp, Warning, TEXT("[INPUT] HandleUsingCandleInput fired. Local=%d HasAuth=%d Role=%d Controller=%s"),
		IsLocallyControlled(),
		HasAuthority(),
		(int32)GetLocalRole(),
		*GetNameSafe(GetController())
	);*/

	if (StatComponent->GetStamina() >= StatComponent->GetMaxStamina())
	{
		return;
	}
	
	if(ItemComponent->GetCandle() <= 0)
	{
		return;
	}
	if(!HasAuthority())
	{
		ServerUseCandle();
		return;
	}
	ServerUseCandle();
}

void AAlkaidCharacter::HandleAttackInput(const FInputActionValue& InValue)
{

}

void AAlkaidCharacter::HandleEscUIInput(const FInputActionValue& InValue)
{
	if (IsValid(EscWidgetClass) == true)
	{
		if (IsValid(EscWidgetInstance))
		{
			EscWidgetInstance->AddToViewport();
			// Add 164~175줄 수정 UI 작업
			// 마우스 커서를 보이게
			APlayerController* UIMouse = Cast<APlayerController>(GetController());
			if (UIMouse)
			{
				FInputModeGameAndUI InputUIMode;
				InputUIMode.SetWidgetToFocus(EscWidgetInstance->TakeWidget()); //포커스 설정, 마우스가 UI를 가리키도록
				InputUIMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); //마우스 고정, 화면 밖으로 못나가게

				UIMouse->SetInputMode(InputUIMode); // 입력우선순위 설정
				UIMouse->bShowMouseCursor = true;
			} //End
		}
	}
}

void AAlkaidCharacter::HandleReadyInput(const FInputActionValue& InValue)
{
	UE_LOG(LogTemp, Warning, TEXT("Ready Action"));
}

void AAlkaidCharacter::HandleStartInput(const FInputActionValue& InValue)
{
	UE_LOG(LogTemp, Warning, TEXT("Started"));
}

void AAlkaidCharacter::StartSprint(const FInputActionValue& Invalue)
{
	if (!IsLocallyControlled())
		return;

	if(StatComponent&& StatComponent->GetStamina() <= 0.0f)
		return;

	bIsSprinting = true;
	if(StatComponent)
		StatComponent->ApplySpeed();

	ServerSetSprinting(true);

	//AK_LOG_SPEED("INPUT StartSprint RPC sent");
}

void AAlkaidCharacter::StopSprint(const FInputActionValue& Invalue)
{
	if (!IsLocallyControlled())
		return;

	bIsSprinting = false;
	if(StatComponent)
		StatComponent->ApplySpeed();

	ServerSetSprinting(false);
	//AK_LOG_SPEED("INPUT StopSprint RPC sent");
}

void AAlkaidCharacter::UsingItemInputStarted(const FInputActionValue& Invalue)
{
	if (EquipmentComponent) EquipmentComponent->UsingItemInputStarted();
}

void AAlkaidCharacter::UsingItemInputCompleted(const FInputActionValue& Invalue)
{
	if (EquipmentComponent) EquipmentComponent->UsingItemInputCompleted();
}

void AAlkaidCharacter::UsingItemInputCanceled(const FInputActionValue& Invalue)
{
	if (EquipmentComponent) EquipmentComponent->UsingItemInputCanceled();
}

void AAlkaidCharacter::OnRep_Pushing()
{
	ApplyPushingMovementSetting(Pushing != nullptr);
}

void AAlkaidCharacter::ServerStopPushing_Implementation()
{
	if (!Pushing)
		return;

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastPushToggleTime < PushToggleCooldown)
		return;
	LastPushToggleTime = Now;

	AActor* Block = Pushing;
	Pushing = nullptr;

	Block->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	if(UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Block->GetRootComponent()))
	{
		PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		PrimComp->SetEnableGravity(true);
		PrimComp->SetSimulatePhysics(true);
	}
	ApplyPushingMovementSetting(false);

	UE_LOG(LogTemp, Warning, TEXT("[Push] STOP. Block=%s"), *GetNameSafe(Block));
}

void AAlkaidCharacter::ServerStartPushing_Implementation(AActor* NewBlock)
{
	if(!NewBlock)
		return;
	if(Pushing)
		return;

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastPushToggleTime < PushToggleCooldown)
		return;
	LastPushToggleTime = Now;

	USkeletalMeshComponent* MeshComp = GetMesh();
	if(!MeshComp)
		return;

	if(!EquipmentComponent)
		return;

	if(!StatComponent)
		return;

	if(StatComponent->GetStamina() <= 0.0f)
		return;

	Pushing = NewBlock;

	const FVector ToBlock = NewBlock->GetActorLocation() - GetActorLocation();
	const FVector ToBlock2D(ToBlock.X, ToBlock.Y, 0.f);

	PushingYaw = ToBlock2D.IsNearlyZero()
		? (Controller ? Controller->GetControlRotation().Yaw : GetActorRotation().Yaw)
		: ToBlock2D.Rotation().Yaw;

	if(UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(NewBlock->GetRootComponent()))
	{
		PrimComp->SetSimulatePhysics(false);
		PrimComp->SetEnableGravity(false);
		PrimComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	const FName SocketName = EquipmentComponent->GetRightHandSocketName();

	NewBlock->AttachToComponent(
		MeshComp,
		FAttachmentTransformRules::KeepWorldTransform,
		SocketName
	);

	NewBlock->SetActorLocation(GetActorLocation() + GetActorForwardVector() * 80.f + FVector(0, 0, 40.f));
	NewBlock->SetActorRotation(FRotator(0.f, PushingYaw, 0.f));

	ApplyPushingMovementSetting(true);

	UE_LOG(LogTemp, Warning, TEXT("[Push] START. Block=%s Socket=%s"),
		*GetNameSafe(NewBlock), *SocketName.ToString());

}

void AAlkaidCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*static float Accum = 0.f;
	Accum += DeltaTime;
	if (Accum >= 0.5f)
	{
		Accum = 0.f;
		AK_LOG_SPEED("TICK heartbeat");
	}*/
	
	if(!HasAuthority())
		return;

	if (bIsSprinting)
	{
		const float Speed2D = GetVelocity().Size2D();
		const bool bIsMoving = Speed2D > 3.0f;
		if (bIsMoving)
		{
			StatComponent->AddStamina(SprintStaminaCost * DeltaTime);
			if (StatComponent->GetStamina() <= 0.0f)
			{
				ServerSetSprinting(false);
			}
		}
	}

	if (EquipmentComponent)
	{
		const bool bCarryingPuzzle = 
			(EquipmentComponent->GetEquipmentType() == EEquipmentType::Puzzle) &&
			(EquipmentComponent->GetHeldItemRight() != nullptr || EquipmentComponent->GetHeldItemLeft() != nullptr);

		if (bCarryingPuzzle)
		{
			StatComponent->AddStamina(EquipmentStaminaCost * DeltaTime);

			if (StatComponent->GetStamina() <= 0.0f)
			{
				EquipmentComponent->ServerDropItem();
				return;
			}
		}
	}

	if(Pushing)
	{
		StatComponent->AddStamina(EquipmentStaminaCost * DeltaTime);

		if (StatComponent->GetStamina() <= 0.0f)
		{
			ServerStopPushing();
			return;
		}
	}
}

// Called to bind functionality to input
void AAlkaidCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::HandleMoveInput);

	EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::HandleLookInput);

	EIC->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

	EIC->BindAction(UsingItem, ETriggerEvent::Started, this, &ThisClass::UsingItemInputStarted);
	EIC->BindAction(UsingItem, ETriggerEvent::Completed, this, &ThisClass::UsingItemInputCompleted);
	EIC->BindAction(UsingItem, ETriggerEvent::Canceled, this, &ThisClass::UsingItemInputCanceled);

	EIC->BindAction(UsingCandle, ETriggerEvent::Started, this, &ThisClass::HandleUsingCandleInput);

	EIC->BindAction(EscUI, ETriggerEvent::Started, this, &ThisClass::HandleEscUIInput);

	EIC->BindAction(ReadyAction, ETriggerEvent::Started, this, &ThisClass::HandleReadyInput);

	EIC->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::HandleAttackInput);

	EIC->BindAction(StartAction, ETriggerEvent::Started, this, &ThisClass::HandleStartInput);

	EIC->BindAction(SprintAction, ETriggerEvent::Started, this, &ThisClass::StartSprint);
	EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &ThisClass::StopSprint);

	
}

