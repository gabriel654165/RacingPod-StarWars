// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"

#include "GameHUD.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT2_API UGameHUD : public UUserWidget
{
	GENERATED_BODY()

public:

	bool CountDown(float delta);
	void SetSpeed(float speed, float maxSpeed);
	void UpdateTimer(float delta);
	void UpdateLap(int currentLap, int totalLaps);
	void UpdatePos();

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* CenterText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* SpeedText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* SpeedProgressBar;


	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* LapText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* TimerText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* PositionText;

protected:

	float m_elapsedTimeStart = 0;
	float m_elapsedTimeRace = 0;

private:

};
