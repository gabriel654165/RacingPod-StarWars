// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "EnhancedInputLibrary.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameHUD.h"
#include "Blueprint/UserWidget.h"
#include "InputActionValue.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/FloatingPawnMovement.h"

#include "AudioMovementDetection.h"

#include "RacingPod.generated.h"

class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class MYPROJECT2_API ARacingPod : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARacingPod();
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPLayReason) override;

    void Accelerate(const FInputActionValue& Value);
    void Rotate(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void SwitchCam(const FInputActionValue& Value);
    void CancelAccelerate();
    void CancelRotate(bool right, bool left);
    void CancelRotateAll();
    void CancelLook();

    void HandleAcceleration(float deltaTime);
    int GetCurrentSpeed(float deltaTime, float maxSpeed);
    void HandleLevitation(bool shouldGoDown, float force);

    int ShouldGoDown();
    void ClampToUpperGround();

    void HandleCameraMoves(float deltaTime);

    UFUNCTION()
    void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    // Inputs
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputMappingContext* RacingPodMappingContext;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* AccelerateAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* RotateAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* LookAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* SwitchCamAction;

    // Move Speed
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxSpeed = 500000;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RegularSpeed = 250000;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinSpeed = 31250;
    float m_currentSpeed = 0;

    // Acceleration
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Acceleration = 10;
    float m_acceleration = MinSpeed;


    // Rotation Speed
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxRotationSpeed = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RegularRotationSpeed = 0.5;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinRotationSpeed = 0.25;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AddRotationSpeed = 2;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ResetRotationSpeed = 1;
    float m_lastRotationPitch;
    float m_currentRotationSpeed;

    // Rotation Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SprintMaxRotationPitch = 70;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SlowMaxRotationPitch = 10;
   
    // Levitation with Raycast
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool UseRaycast = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LevitationUpForce = 50000;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LevitationDownForce = 100000;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxLevitationHeight = 300;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinLevitationHeight = 250;

    // Camera moves
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AlphaCameraResetRotationPos = 0.1f;
    
    //FOV
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FOVMaxSpeed = 130;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FOVRegSpeed = 90;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FOVMinSpeed = 70;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TimerFieldOfViewChange = 2;
    float srcFieldOfView = 90;
    float elapsedTimeFieldOfViewChange = 0;

    // Debug Raycasts
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool DebugRaycast = false;

    // Camera
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector CameraOffset = FVector(0, 0, 0);
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector CameraOffset2 = FVector(0, 0, 0);
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector CameraOffset3 = FVector(0, 0, 0);
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int CameraIndex = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int NbCameraPos = 3;

    //Pods
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int CountLeftPods = 2;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int CountRightPods = 2;

    //Physics
    UFloatingPawnMovement* m_movementComponent;

    //Sounds
    UAudioMovementDetection* m_audioComponent;
    //UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Audio")
    //USoundBase* soundToPlay;
    //UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Audio")
    //UMetaSoundSource* soundToPlay2;

    //HUD
    UPROPERTY(EditAnyWhere)
    TSubclassOf<class UGameHUD> PlayerHUDClass;
    UPROPERTY()
    class UGameHUD* PlayerHUD;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxSpeedHUD = 50000;

    //Game states
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int TotalLaps = 3;
    bool m_gameStarted = false;
    bool m_gameEnded = false;
    int m_currentLap = 0;

private:
    // Speed
    FVector lastFramePosition;
    float speedSumForAverage;
    TArray<float> speedHistory;
    int speedHistoryLength;

    UPROPERTY(VisibleAnywhere)
    UCapsuleComponent* Capsule;

    UPROPERTY(VisibleAnywhere)
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere)
    UCameraComponent* ViewCamera;

    //Motors
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* BoostLeft0;
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* BoostLeft1;
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* BoostRight0;
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* BoostRight1;
    
    //Meshes
    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* PodLeftMesh0;
    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* PodLeftMesh1;
    TArray<USkeletalMeshComponent*> PodLeftMeshes;

    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* PodRightMesh0;
    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* PodRightMesh1;
    TArray<USkeletalMeshComponent*> PodRightMeshes;

    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* CockpitMesh;

};
