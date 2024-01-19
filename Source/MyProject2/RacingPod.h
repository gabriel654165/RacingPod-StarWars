// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "EnhancedInputLibrary.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "GameFramework/FloatingPawnMovement.h"

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

    void Accelerate(const FInputActionValue& Value);
    void Rotate(const FInputActionValue& Value);
    void CancelAccelerate();
    void CancelRotate();

    void HandleGravity();
    void HandleAcceleration();

    // Inputs
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputMappingContext* RacingPodMappingContext;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* AccelerateAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* RotateAction;

    // Move Speed
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxSpeed = 50000;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RegularSpeed = 5000;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinSpeed = 1000;
    float m_currentSpeed = MinSpeed;

    // Rotation Speed
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxRotationSpeed = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RegularRotationSpeed = 0.5;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinRotationSpeed = 0.25;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ResetRotationSpeed = 1;
    float m_currentRotationSpeed;

    // Rotation Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SprintMaxRotationPitch = 70;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SlowMaxRotationPitch = 10;

    // Acceleration / S
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Acceleration = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float GravityScale = 50;

    UFloatingPawnMovement* m_movementComponent;


private:
    UPROPERTY(VisibleAnywhere)
    UCapsuleComponent* Capsule;

    UPROPERTY(VisibleAnywhere)
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere)
    UCameraComponent* ViewCamera;

    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* RacingPodMesh;

};
