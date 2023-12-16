// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "EnhancedInputLibrary.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "RacingPod.generated.h"
//#include "C:\Users\Gabriel\source\repos\MyProject2\Intermediate\Build\Win64\UnrealEditor\Inc\MyProject2\UHT\RacingPod.generated.h"

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputMappingContext* RacingPodMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
    UInputAction* LookAction;

    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);

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
