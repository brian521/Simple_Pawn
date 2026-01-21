// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"
#include "MyPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RootComponent"));
	CapsuleComponent->SetSimulatePhysics(false);
	SetRootComponent(CapsuleComponent);

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComp->SetupAttachment(CapsuleComponent);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Dark_Knight/Dark_Knight_Male/Meshes/Parts/SKM_DKM_Helmet.SKM_DKM_Helmet"));
	SkeletalMeshComp->SetSkeletalMesh(MeshAsset.Object);
	SkeletalMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -155.0f));
	SkeletalMeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	SkeletalMeshComp->SetSimulatePhysics(false);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));

	SpringArmComp->SetupAttachment(CapsuleComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = false;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;


}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}


// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&AMyPawn::Move
				);

				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Completed,
					this,
					&AMyPawn::StopMove
				);
			}
			
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&AMyPawn::Look
				);

				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Completed,
					this,
					&AMyPawn::StopLook
				);
			}
		}
	}
}

void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!MoveInput.IsNearlyZero())
	{
		//FVector MoveDirection =	GetActorForwardVector() * MoveInput.X + GetActorRightVector() * MoveInput.Y;

		//MoveDirection.Normalize();

		//FVector DeltaMove = MoveDirection * MoveSpeed * DeltaTime;
		FVector DeltaMove =	FVector(MoveInput.X, MoveInput.Y, 0.f) * MoveSpeed * DeltaTime;
		AddActorLocalOffset(DeltaMove, true);
	}

	if (!LookInput.IsNearlyZero())
	{
		CurrentYaw += LookInput.X * LookSpeed * DeltaTime;
		CurrentPitch += LookInput.Y * LookSpeed * DeltaTime;

		CurrentPitch = FMath::Clamp(CurrentPitch, -80.0f, 80.0f);

		SetActorRotation(FRotator(0.0f, CurrentYaw, 0.0f));

		SpringArmComp->SetRelativeRotation(FRotator(CurrentPitch, 0.0f, 0.0f));
	}
}

void AMyPawn::Move(const FInputActionValue& value)
{
	MoveInput = value.Get<FVector2D>();
}

void AMyPawn::StopMove(const FInputActionValue& value)
{
	MoveInput = FVector2D::ZeroVector;
}


void AMyPawn::Look(const FInputActionValue& value)
{
	LookInput = value.Get<FVector2D>();
}

void AMyPawn::StopLook(const FInputActionValue& value)
{
	LookInput = FVector2D::ZeroVector;
}

