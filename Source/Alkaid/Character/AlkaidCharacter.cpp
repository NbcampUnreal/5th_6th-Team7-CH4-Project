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
#include "Components/PrimitiveComponent.h"
#include "Items/AKComponents/ItemComponent.h"

// Sets default values
AAlkaidCharacter::AAlkaidCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
}

void AAlkaidCharacter::ServerUseCandle_Implementation()
{
	if (!StatComponent)
		return; 
	
	if(StatComponent->GetCandleCount() <= 0)
		return;

	if (StatComponent->IsCandleOnCooldown())
		return;

	StatComponent->StartCandleCooldown();
	StatComponent->AddCandleCount(-1);
	StatComponent->AddStamina(20.0f);
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
	if(StatComponent->GetCandleCount() <= 0)
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

// Called every frame
void AAlkaidCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
}

