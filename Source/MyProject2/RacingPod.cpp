// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingPod.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

// Sets default values
ARacingPod::ARacingPod()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // CAPSULE
    Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
    Capsule->SetCapsuleHalfHeight(1000.f);
    Capsule->SetCapsuleRadius(1000.f);
    SetRootComponent(Capsule);

    // SPRING ARM
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(GetRootComponent());

    // CAMERA
    ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
    ViewCamera->SetupAttachment(SpringArm);

    // SKELETAL MESH
    RacingPodMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RacingPodMesh"));
    RacingPodMesh->SetupAttachment(GetRootComponent());
    
    // PAWN MOVEMENT
    m_movementComponent = CreateDefaultSubobject<UFloatingPawnMovement>("RacingPodMovement");
    m_movementComponent->MaxSpeed = RegularSpeed;
    m_currentSpeed = MinSpeed;
    m_currentRotationSpeed = MaxRotationSpeed;

    AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ARacingPod::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(RacingPodMappingContext, 0);
        }
    }
}

// Called every frame
void ARacingPod::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (Controller)
    {
        //HandleGravity();
        HandleAcceleration();
    }
}

void ARacingPod::HandleGravity()
{
    FVector Up = GetActorUpVector();
    AddMovementInput(Up, -1.0f * GravityScale);
}

void ARacingPod::HandleAcceleration()
{
    //if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("%i"), m_currentSpeed)); }
    FVector Forward = GetActorForwardVector();

    m_movementComponent->MaxSpeed = m_currentSpeed;
    //soucis -> je add une value
    AddMovementInput(Forward, m_currentSpeed);
}

void ARacingPod::Accelerate(const FInputActionValue& Value)
{
    const float CurrentValue = Value.Get<float>();

    if (!Controller)
    {
        return;
    }
    if (CurrentValue != 0.f)
    {
        if (CurrentValue > 0)
        {
            m_currentRotationSpeed = MinRotationSpeed;
            m_currentSpeed = MaxSpeed;
        }
        else
        {
            m_currentRotationSpeed = MaxRotationSpeed;
            m_currentSpeed = MinSpeed;
        }
    }
}

void ARacingPod::CancelAccelerate()
{
    if (GEngine)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("NOT ACCEL")));
    }
}

void ARacingPod::Rotate(const FInputActionValue& Value)
{
    const float CurrentValue = Value.Get<float>();

    if (Controller && (CurrentValue != 0.f))
    {
        FRotator rotationPod = FRotator(0.f, CurrentValue * m_currentRotationSpeed, 0.f);
        AddActorWorldRotation(rotationPod);

        //rotation between 70 -> -70 
        // @todo : calculate the max Pitch from accel : AccelMax -> PitchMax
        if (RacingPodMesh->GetComponentRotation().Pitch > 70 && CurrentValue > 0)
        {
            // DO NOTHING
        }
        else if (RacingPodMesh->GetComponentRotation().Pitch < -70 && CurrentValue < 0)
        {
            // DO NOTHING
        }
        else
        {
            RacingPodMesh->USceneComponent::AddLocalRotation(FRotator(CurrentValue, 0.f, 0.f));
        }
    }
}

void ARacingPod::CancelRotate()
{
    const float FactorRotation = 1 * ResetRotationSpeed;

    if (int(abs(RacingPodMesh->GetComponentRotation().Pitch)) == 0)
    {
        return;
    }
    else if (RacingPodMesh->GetComponentRotation().Pitch > 0.f)
    {
        RacingPodMesh->USceneComponent::AddLocalRotation(FRotator(-FactorRotation, 0.f, 0.f));
    }
    else if (RacingPodMesh->GetComponentRotation().Pitch < 0.f)
    {
        RacingPodMesh->USceneComponent::AddLocalRotation(FRotator(FactorRotation, 0.f, 0.f));
    }
}

/*void ARacingPod::Look(const FInputActionValue& Value)
{
    const FVector2D LookAxisValue = Value.Get<FVector2D>();

    // if (GEngine)
    // {
    //     GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Hello world %i"), LookAxisValue.Y));
    // }

    if (GetController())
    {
        AddControllerYawInput(LookAxisValue.X);
        AddControllerPitchInput(LookAxisValue.Y * (- 1));
    }
}*/

// Called to bind functionality to input
void ARacingPod::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(AccelerateAction, ETriggerEvent::Triggered, this, &ARacingPod::Accelerate);
        EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &ARacingPod::Rotate);

        EnhancedInputComponent->BindAction(AccelerateAction, ETriggerEvent::None, this, &ARacingPod::CancelAccelerate);
        EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::None, this, &ARacingPod::CancelRotate);
    }
}

