// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AlkaidCharacter.h"
#include "Controller/AlkaidPlayerController.h"
#include "Character/AlkaidCharaterStatComponent.h"
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
		checkf(IsValid(PC) == true, TEXT("PlayerController is invalid."));

		UEnhancedInputLocalPlayerSubsystem* EILPS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		checkf(IsValid(EILPS) == true, TEXT("EnhancedInputLocalPlayerSubsystem is invalid."));

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

void AAlkaidCharacter::SprintSpeed_Server()
{
	if (!HasAuthority())
		return;
	if (!StatComponent)
		return;

	const float NewSpeed = StatComponent->GetFinalMoveSpeed(bIsSprinting);
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

void AAlkaidCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAlkaidCharacter, bIsSprinting);
}

void AAlkaidCharacter::ServerSetSprinting_Implementation(bool NewSprinting)
{
	bIsSprinting = NewSprinting;

	SprintSpeed_Server();
}

void AAlkaidCharacter::HandleMoveInput(const FInputActionValue& InValue)
{
	if(IsValid(Controller) == false)
	{
		return;
	}

	const FVector2D InMovementVector = InValue.Get<FVector2D>();

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator ControlYawRotation(0.0f, ControlRotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, InMovementVector.X);
	AddMovementInput(RightDirection, InMovementVector.Y);
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

void AAlkaidCharacter::HandleUsingItemInput(const FInputActionValue& InValue)
{
	
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

	ServerSetSprinting(true);
}

void AAlkaidCharacter::StopSprint(const FInputActionValue& Invalue)
{
	if (!IsLocallyControlled())
		return;

	ServerSetSprinting(false);
}

// Called every frame
void AAlkaidCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(!HasAuthority())
		return;

	if (bIsSprinting)
	{
		const float Speed2D = GetVelocity().Size2D();
		const bool bIsMoving = Speed2D > 3.0f;
		if (bIsMoving)
		{
			StatComponent->AddStamina(-6 * DeltaTime);
			if (StatComponent->GetStamina() <= 0.0f)
			{
				bIsSprinting = false;
				SprintSpeed_Server();
			}
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

	EIC->BindAction(UsingItem, ETriggerEvent::Started, this, &ThisClass::HandleUsingItemInput);

	EIC->BindAction(UsingCandle, ETriggerEvent::Started, this, &ThisClass::HandleUsingCandleInput);

	EIC->BindAction(EscUI, ETriggerEvent::Started, this, &ThisClass::HandleEscUIInput);

	EIC->BindAction(ReadyAction, ETriggerEvent::Started, this, &ThisClass::HandleReadyInput);

	EIC->BindAction(AttackAction, ETriggerEvent::Started, this, &ThisClass::HandleAttackInput);

	EIC->BindAction(StartAction, ETriggerEvent::Started, this, &ThisClass::HandleStartInput);

	EIC->BindAction(SprintAction, ETriggerEvent::Started, this, &ThisClass::StartSprint);
	EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &ThisClass::StopSprint);
}

