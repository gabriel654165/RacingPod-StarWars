// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"

#include "AudioMovementDetection.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT2_API UAudioMovementDetection : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAudioMovementDetection();

	void Play();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FVector lastFramePosition;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Speed")
	float currentRawSpeed;

	float currentSmoothSpeed;
	float speedSumForAverage;
	TArray<float> speedHistory;
	int speedHistoryLength;

	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Audio")
	//USoundBase* soundToPlay;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Speed")
	float smoothSpeed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Speed")
	float maxSpeed;
};
