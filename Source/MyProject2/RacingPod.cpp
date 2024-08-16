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
    //Capsule->SetCapsuleHalfHeight(1000.f);
    //Capsule->SetCapsuleRadius(1000.f);
    SetRootComponent(Capsule);

    // SPRING ARM
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(GetRootComponent());

    // CAMERA
    ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
    ViewCamera->SetupAttachment(SpringArm);
    ViewCamera->SetRelativeLocation(CameraOffset);

    // Meshes
    PodLeftMesh0 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PodLeftMesh0"));
    PodLeftMesh0->SetupAttachment(GetRootComponent());
    PodLeftMesh1 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PodLeftMesh1"));
    PodLeftMesh1->SetupAttachment(GetRootComponent());
    PodLeftMeshes.Add(PodLeftMesh0);
    PodLeftMeshes.Add(PodLeftMesh1);

    PodRightMesh0 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PodRightMesh0"));
    PodRightMesh0->SetupAttachment(GetRootComponent());
    PodRightMesh1 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PodRightMesh1"));
    PodRightMesh1->SetupAttachment(GetRootComponent());
    PodRightMeshes.Add(PodRightMesh0);
    PodRightMeshes.Add(PodRightMesh1);

    //BoostLeft0 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoostLeft0"));
    //BoostLeft0->SetupAttachment(PodLeftMesh0);
    //BoostLeft1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoostLeft1"));
    //BoostLeft1->SetupAttachment(PodLeftMesh1);

    //BoostRight0 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoostRight0"));
    //BoostLeft0->SetupAttachment(PodRightMesh0);
    //BoostRight1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoostRightt1"));
    //BoostLeft0->SetupAttachment(PodRightMesh1);

    CockpitMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CockpitMesh"));
    CockpitMesh->SetupAttachment(GetRootComponent());

    // PAWN MOVEMENT
    m_movementComponent = CreateDefaultSubobject<UFloatingPawnMovement>("RacingPodMovement");
    m_movementComponent->MaxSpeed = MaxSpeed;
    m_acceleration = MinSpeed;
    m_currentSpeed = MinSpeed;
    m_currentRotationSpeed = MaxRotationSpeed;
    speedSumForAverage = 0;
    speedHistoryLength = 10;

    // AUDIO
    //m_audioComponent = CreateDefaultSubobject<UAudioMovementDetection>("AudioMovementDetection");
    //m_audioComponent->soundToPlay = soundToPlay;
    //m_audioComponent->maxSpeed = MaxSpeed;

    //HUD
    PlayerHUDClass = nullptr;
    PlayerHUD = nullptr;

    AutoPossessPlayer = EAutoReceiveInput::Player0;
}


void ARacingPod::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PlayerController = GetController<APlayerController>())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(RacingPodMappingContext, 0);
        }

        if (IsLocallyControlled() && PlayerHUDClass)
        {
            check(PlayerController);
            PlayerHUD = CreateWidget<UGameHUD>(PlayerController, PlayerHUDClass);
            check(PlayerHUD);
            PlayerHUD->AddToPlayerScreen();
            PlayerHUD->UpdateLap(m_currentLap, TotalLaps);
        }
    }

    Capsule->OnComponentBeginOverlap.AddDynamic(this, &ARacingPod::BeginOverlap);
    m_lastRotationPitch = PodLeftMeshes[0]->GetComponentRotation().Pitch;
    
    // AUDIO
    //m_audioComponent->soundToPlay = soundToPlay;
    //m_audioComponent->maxSpeed = MaxSpeed;
    //m_audioComponent->Play();
}

void ARacingPod::EndPlay(const EEndPlayReason::Type EndPLayReason)
{
    if (PlayerHUD)
    {
        PlayerHUD->RemoveFromParent();
        PlayerHUD = nullptr;
    }

    Super::EndPlay(EndPLayReason);
}

void ARacingPod::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (Controller)
    {
        if (m_currentLap > TotalLaps && !m_gameEnded) {
            m_gameEnded = true;
        }

        if (PlayerHUD && !m_gameStarted && !m_gameEnded) {
            m_gameStarted = PlayerHUD->CountDown(DeltaTime);
        }

        if (m_gameStarted && !m_gameEnded) {
            HandleAcceleration(DeltaTime);
            PlayerHUD->UpdateTimer(DeltaTime);
            HandleCameraMoves(DeltaTime);
        }
        
        //if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Acc = %f"), m_movementComponent->Acceleration)); }
        //if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Vel = %s"), *m_movementComponent->Velocity.ToString() )); }
        
        // AUDIO
        //m_audioComponent->maxSpeed = m_movementComponent->MaxSpeed;

        if (UseRaycast) {
            int dirToPushIndex = ShouldGoDown();

            if (dirToPushIndex == -1) {
                HandleLevitation(true, LevitationDownForce);
            }
            else if (dirToPushIndex == 1) {
                HandleLevitation(false, LevitationUpForce);
            }
        }
    }
}

void ARacingPod::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    //if (OtherComp)
    //if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Overlap"))); }

    m_currentLap += 1;

    PlayerHUD->UpdateLap(m_currentLap, TotalLaps);
}

void ARacingPod::HandleCameraMoves(float deltaTime)
{
    if (elapsedTimeFieldOfViewChange + deltaTime >= TimerFieldOfViewChange) {
        elapsedTimeFieldOfViewChange = TimerFieldOfViewChange;
    }
    else {
        elapsedTimeFieldOfViewChange += deltaTime;
    }

    if (m_acceleration >= MinSpeed && m_acceleration < RegularSpeed) {
        float newValue = FMath::Lerp(srcFieldOfView, FOVMinSpeed, elapsedTimeFieldOfViewChange / TimerFieldOfViewChange);

        ViewCamera->FieldOfView = newValue;
        return;
    }

    if (m_currentRotationSpeed == RegularRotationSpeed) {
        float newValue = FMath::Lerp(srcFieldOfView, FOVRegSpeed, elapsedTimeFieldOfViewChange / TimerFieldOfViewChange);

        ViewCamera->FieldOfView = newValue;
        return;
    }

    if (m_acceleration > RegularSpeed && m_acceleration <= MaxSpeed) {
        float newValue = FMath::Lerp(srcFieldOfView, FOVMaxSpeed, elapsedTimeFieldOfViewChange / TimerFieldOfViewChange);
        
        ViewCamera->FieldOfView = newValue;
        return;
    }
}

void ARacingPod::HandleAcceleration(float deltaTime)
{
    FVector Forward = GetActorForwardVector();

    m_currentSpeed = GetCurrentSpeed(deltaTime, MaxSpeedHUD);
    PlayerHUD->SetSpeed(m_currentSpeed, MaxSpeedHUD);

    m_movementComponent->MaxSpeed = m_acceleration;
    AddMovementInput(Forward, m_acceleration);
}

int ARacingPod::GetCurrentSpeed(float deltaTime, float maxSpeed)
{
    FVector currentPosition = GetActorLocation();
    float currentRawSpeed = FVector::Dist(currentPosition, lastFramePosition) / deltaTime;

    speedSumForAverage += currentRawSpeed;
    speedHistory.Add(currentRawSpeed);

    if (speedHistory.Num() > speedHistoryLength)
    {
        speedSumForAverage -= speedHistory[0];
        speedHistory.RemoveAt(0);
    }

    float currentSmoothSpeed = speedSumForAverage / FMath::Min(speedHistoryLength, speedHistory.Num());
    currentRawSpeed = FMath::Clamp(currentSmoothSpeed / maxSpeed, 0.0f, 1.0f);

    //if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("RAW %f"), currentRawSpeed)); }
    //if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("SMOOTH %f"), currentSmoothSpeed)); }

    lastFramePosition = currentPosition;

    return currentSmoothSpeed;
}

void ARacingPod::HandleLevitation(bool pushDown = true, float force = 0)
{
    //if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, pushDown ? "---> DOWN" : "---> UP"); }
    FVector Direction = (pushDown ? -1.f : 1.f) * GetActorUpVector();

    //m_movementComponent->MaxSpeed = force;
    AddMovementInput(Direction, force);
}

void ARacingPod::ClampToUpperGround()
{
    FVector start = GetActorLocation();
    FVector end = start + (GetActorUpVector() * 1000000);
    FHitResult hit;

    if (GetWorld())
    {
        bool actorHit = GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Pawn, FCollisionQueryParams(), FCollisionResponseParams());
        
        if (DebugRaycast) { DrawDebugLine(GetWorld(), start, end, FColor::Blue, false, 2.f, 0.f, 10.f); }

        if (actorHit && hit.GetActor() != this)
        {
            auto hitPosZ = hit.ImpactPoint.Z;
            auto hitPos = FVector(GetActorLocation().X, GetActorLocation().Y, hitPosZ);
            FVector lerpedHitPos = FMath::Lerp(GetActorLocation(), hitPos, 0.1f);

            //if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("----CLAMP--- : %s"), *lerpedHitPos.ToString())); }

            SetActorLocation(hitPos, false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
            //SetActorLocation(lerpedHitPos, false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
            //SetActorLocation(lerpedHitPos, true, &hit, ETeleportType::TeleportPhysics);
            //SetActorLocation(hitPos, true, &hit, ETeleportType::TeleportPhysics);
        }
    }
}

int ARacingPod::ShouldGoDown()
{
    FVector start = GetActorLocation();
    FVector end = start + (-GetActorUpVector() * 1000000);
    FHitResult hit;

    if (GetWorld())
    {
        bool actorHit = GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Pawn, FCollisionQueryParams(), FCollisionResponseParams());
        
        //if (hit.GetActor() == this) { if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "collide avec toi"); }}

        if (actorHit && hit.GetActor() && hit.GetActor() != this)
        {
            //if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Distance = %i"), hit.Distance));}
            //if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("1 = %s"), *hit.GetActor()->GetFName().ToString())); }

            if (hit.Distance > MinLevitationHeight && hit.Distance < MaxLevitationHeight) {
                //if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, "0 Nothing"); }
                if (DebugRaycast) { DrawDebugLine(GetWorld(), start, end, FColor::Blue, false, 2.f, 0.f, 10.f); }
                return 0;
            }
            else if (hit.Distance < MinLevitationHeight) {
                //if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "1 Up"); }
                if (DebugRaycast) { DrawDebugLine(GetWorld(), start, end, FColor::Green, false, 2.f, 0.f, 10.f); }
                return 1;
            }
            else if (hit.Distance > MaxLevitationHeight) {
                //if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "-1 Down"); }
                if (DebugRaycast) { DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 2.f, 0.f, 10.f); }
                return -1;
            }
        }
        // No collision == too low
        else if (hit.GetActor() != this) {
            ClampToUpperGround();
            return 0;
        }
        //else if (hit.GetActor() == this) {
        //   if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "COLLIDE ITSELF"); }
        //}

    } 
    return 0;
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
            bool speedHasChanged = m_acceleration != MinRotationSpeed;
            
            if (speedHasChanged) {
                elapsedTimeFieldOfViewChange = 0;
                srcFieldOfView = ViewCamera->FieldOfView;
            }
            m_currentRotationSpeed = MinRotationSpeed;
            m_acceleration = MaxSpeed;
        }
        else
        {
            bool speedHasChanged = m_acceleration != MaxRotationSpeed;

            if (speedHasChanged) {
                elapsedTimeFieldOfViewChange = 0;
                srcFieldOfView = ViewCamera->FieldOfView;
            }
            m_currentRotationSpeed = MaxRotationSpeed;
            m_acceleration = MinSpeed;
        }
    }
}

void ARacingPod::CancelAccelerate()
{
    bool speedHasChanged = m_acceleration != RegularRotationSpeed;

    if (speedHasChanged) {
        elapsedTimeFieldOfViewChange = 0;
        srcFieldOfView = ViewCamera->FieldOfView;
    }

    m_currentRotationSpeed = RegularRotationSpeed;
    m_acceleration = RegularSpeed;
}

void ARacingPod::Rotate(const FInputActionValue& Value)
{
    const float CurrentValue = Value.Get<float>();
    const float FactorActorRotation = CurrentValue * m_currentRotationSpeed;
    const float FactorMotorsRotation = CurrentValue * AddRotationSpeed;

    if (Controller && ((int)CurrentValue != 0.f))
    {
        //rotate actor
        FRotator rotationPod = FRotator(0.f, FactorActorRotation, 0.f);
        AddActorWorldRotation(rotationPod);

        //rotate components
        double rotationPitch = PodLeftMeshes[0]->GetComponentRotation().Pitch;
        ///* MOVE ACTOR */double rotationPitch = GetActorRotation().Roll;

        if (rotationPitch > 70 && CurrentValue > 0) {}
        else if (rotationPitch < -70 && CurrentValue < 0) {}
        else
        {
            if (CurrentValue > 0) {
                PodLeftMeshes[0]->USceneComponent::AddLocalRotation(FRotator(FactorMotorsRotation, 0.f, 0.f));
                PodLeftMeshes[1]->USceneComponent::AddLocalRotation(FRotator(FactorMotorsRotation, 0.f, 0.f));

                PodRightMeshes[0]->USceneComponent::AddLocalRotation(FRotator(FactorMotorsRotation, 0.f, 0.f));
                PodRightMeshes[1]->USceneComponent::AddLocalRotation(FRotator(FactorMotorsRotation, 0.f, 0.f));
            }
            else if (CurrentValue < 0) {
                CancelRotate(false, false);
            }
            ///* MOVE ACTOR */AddActorLocalRotation(FRotator(0.f, 0.f, CurrentValue));
        }

        rotationPitch = PodRightMeshes[0]->GetComponentRotation().Pitch;

        if (rotationPitch > 70 && CurrentValue > 0) {}
        else if (rotationPitch < -70 && CurrentValue < 0) {}
        else
        {
            if (CurrentValue > 0) {
                CancelRotate(false, false);
            }
            else if (CurrentValue < 0) {
                PodLeftMeshes[0]->USceneComponent::AddLocalRotation(FRotator(FactorMotorsRotation, 0.f, 0.f));
                PodLeftMeshes[1]->USceneComponent::AddLocalRotation(FRotator(FactorMotorsRotation, 0.f, 0.f));
                
                PodRightMeshes[0]->USceneComponent::AddLocalRotation(FRotator(FactorMotorsRotation, 0.f, 0.f));
                PodRightMeshes[1]->USceneComponent::AddLocalRotation(FRotator(FactorMotorsRotation, 0.f, 0.f));
            }
            ///* MOVE ACTOR */AddActorLocalRotation(FRotator(0.f, 0.f, CurrentValue));
        }
    } else {
        CancelRotate(true, true);
    }
}

void ARacingPod::CancelRotate(bool right = true, bool left = false)
{
    const float FactorRotation = 1 * ResetRotationSpeed;

    if (left) {
        double rotationPitch = PodLeftMeshes[0]->GetComponentRotation().Pitch;
    
        if (int(abs(rotationPitch)) == 0)
        {
        }
        else if (rotationPitch > 0.f)
        {
            PodLeftMeshes[0]->USceneComponent::AddLocalRotation(FRotator(-FactorRotation * 2, 0.f, 0.f));
            PodLeftMeshes[1]->USceneComponent::AddLocalRotation(FRotator(-FactorRotation * 2, 0.f, 0.f));
        }
        else if (rotationPitch < 0.f)
        {
            PodLeftMeshes[0]->USceneComponent::AddLocalRotation(FRotator(FactorRotation * 2, 0.f, 0.f));
            PodLeftMeshes[1]->USceneComponent::AddLocalRotation(FRotator(FactorRotation * 2, 0.f, 0.f));
        }
    }


    if (right) {
        double rotationPitch = PodRightMeshes[0]->GetComponentRotation().Pitch;
    
        if (int(abs(rotationPitch)) == 0)
        {
        }
        else if (rotationPitch > 0.f)
        {
            PodRightMeshes[0]->USceneComponent::AddLocalRotation(FRotator(-FactorRotation * 2, 0.f, 0.f));
            PodRightMeshes[1]->USceneComponent::AddLocalRotation(FRotator(-FactorRotation * 2, 0.f, 0.f));
        }
        else if (rotationPitch < 0.f)
        {
            PodRightMeshes[0]->USceneComponent::AddLocalRotation(FRotator(FactorRotation * 2, 0.f, 0.f));
            PodRightMeshes[1]->USceneComponent::AddLocalRotation(FRotator(FactorRotation * 2, 0.f, 0.f));
        }
    }

}

void ARacingPod::CancelRotateAll()
{
    const float FactorRotation = 1 * ResetRotationSpeed;

    double rotationPitch = PodLeftMeshes[0]->GetComponentRotation().Pitch;

    if (int(abs(rotationPitch)) == 0)
    {
    }
    else if (rotationPitch > 0.f)
    {
        PodLeftMeshes[0]->USceneComponent::AddLocalRotation(FRotator(-FactorRotation * 2, 0.f, 0.f));
        PodLeftMeshes[1]->USceneComponent::AddLocalRotation(FRotator(-FactorRotation * 2, 0.f, 0.f));
    }
    else if (rotationPitch < 0.f)
    {
        PodLeftMeshes[0]->USceneComponent::AddLocalRotation(FRotator(FactorRotation * 2, 0.f, 0.f));
        PodLeftMeshes[1]->USceneComponent::AddLocalRotation(FRotator(FactorRotation * 2, 0.f, 0.f));
    }

    rotationPitch = PodRightMeshes[0]->GetComponentRotation().Pitch;

    if (int(abs(rotationPitch)) == 0)
    {
    }
    else if (rotationPitch > 0.f)
    {
        PodRightMeshes[0]->USceneComponent::AddLocalRotation(FRotator(-FactorRotation * 2, 0.f, 0.f));
        PodRightMeshes[1]->USceneComponent::AddLocalRotation(FRotator(-FactorRotation * 2, 0.f, 0.f));
    }
    else if (rotationPitch < 0.f)
    {
        PodRightMeshes[0]->USceneComponent::AddLocalRotation(FRotator(FactorRotation * 2, 0.f, 0.f));
        PodRightMeshes[1]->USceneComponent::AddLocalRotation(FRotator(FactorRotation * 2, 0.f, 0.f));
    }
}

void ARacingPod::Look(const FInputActionValue& Value)
{
    const FVector2D LookAxisValue = Value.Get<FVector2D>();

    //if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("X = %f"), LookAxisValue.X)); }
    //if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Y = % i"), LookAxisValue.Y)); }

    if (GetController())
    {
        if ((- 0.2f > LookAxisValue.X || LookAxisValue.X > 0.2f) || (-0.2f > LookAxisValue.Y || LookAxisValue.Y > 0.2f)) {
            if (-0.2f > LookAxisValue.X || LookAxisValue.X > 0.2f) {
                AddControllerYawInput(LookAxisValue.X);
            }
            if (-0.2f > LookAxisValue.Y || LookAxisValue.Y > 0.2f) {
                AddControllerPitchInput(LookAxisValue.Y * (-1.f));
            }
        }
        else {
            CancelLook();
        }
    }
}

void ARacingPod::CancelLook()
{
    double currentRotationPitch = PodLeftMeshes[0]->GetComponentRotation().Pitch;

    if (m_acceleration <= 0 && abs((int)m_lastRotationPitch) == abs((int)currentRotationPitch)) {
        m_lastRotationPitch = abs((int)currentRotationPitch);
        return;
    }
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    FRotator currentControlRotation = GetControlRotation();
    FRotator currentActorRotation = GetActorRotation();

    if (PlayerController) {
        FRotator NewRotation = FRotator(FQuat::Slerp(currentControlRotation.Quaternion(), currentActorRotation.Quaternion(), AlphaCameraResetRotationPos));

        PlayerController->SetControlRotation(NewRotation);
    }
}

void ARacingPod::SwitchCam(const FInputActionValue& Value)
{
    //if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Value = %s"), *Value.ToString())); }

    if (CameraIndex + 1 > NbCameraPos - 1) {
        CameraIndex = 0;
    }
    else {
        CameraIndex += 1;
    }
    
    /*if (CameraIndex == 0) {
        if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Cam 1"); }
        ViewCamera->SetRelativeLocation(CameraOffset);
    }
    if (CameraIndex == 1) {
        if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Cam 2"); }
        ViewCamera->SetRelativeLocation(CameraOffset2);
    }
    if (CameraIndex == 2) {
        if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Cam 3"); }
        ViewCamera->SetRelativeLocation(CameraOffset3);
    }*/
    if (CameraIndex >= 0 && CameraIndex <= 2) {
        FVector CameraOffsets[] = { CameraOffset, CameraOffset2, CameraOffset3 };

        ViewCamera->SetRelativeLocation(CameraOffsets[CameraIndex]);
    }

}


// Called to bind functionality to input
void ARacingPod::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(AccelerateAction, ETriggerEvent::Triggered, this, &ARacingPod::Accelerate);
        EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &ARacingPod::Rotate);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARacingPod::Look);
        EnhancedInputComponent->BindAction(SwitchCamAction, ETriggerEvent::Started, this, &ARacingPod::SwitchCam);

        EnhancedInputComponent->BindAction(AccelerateAction, ETriggerEvent::None, this, &ARacingPod::CancelAccelerate);
        EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::None, this, &ARacingPod::CancelRotateAll);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::None, this, &ARacingPod::CancelLook);
    }
}

