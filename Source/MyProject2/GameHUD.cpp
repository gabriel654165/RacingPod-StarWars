// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUD.h"


bool UGameHUD::CountDown(float delta)
{
	m_elapsedTimeStart += delta;

	/*if (m_elapsedTimeStart > 0 && m_elapsedTimeStart < 1) {
		CenterText->SetText(FText::FromString("3"));
	}
	else if (m_elapsedTimeStart >= 1 && m_elapsedTimeStart < 2) {
		CenterText->SetText(FText::FromString("2"));
	}
	else if (m_elapsedTimeStart >= 2 && m_elapsedTimeStart < 3) {
		CenterText->SetText(FText::FromString("1"));
	}
	else if (m_elapsedTimeStart >= 3 && m_elapsedTimeStart < 4) {
		CenterText->SetText(FText::FromString("GO"));
	}
	else if (m_elapsedTimeStart >= 4) {
		CenterText->SetText(FText::FromString(""));
	}*/

	if (m_elapsedTimeStart >= 0 && m_elapsedTimeStart < 4) {
		int countdown = 4 - static_cast<int>(m_elapsedTimeStart);
		CenterText->SetText(countdown > 0 ? FText::FromString(FString::FromInt(countdown)) : FText::FromString("GO"));
	}
	else {
		CenterText->SetText(FText::FromString(""));
	}

	if (m_elapsedTimeStart <= 4) {
		return false;
	}
	return true;
}

void UGameHUD::SetSpeed(float speed, float maxSpeed)
{
	if (speed > maxSpeed) {
		speed = maxSpeed;
	}

	float prct = speed / maxSpeed;

	//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("prct : %f"), prct)); }
	//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "========================="); }

	float prctBar = speed / maxSpeed * 0.65;
	UMaterialInstanceDynamic* material = SpeedProgressBar->GetDynamicMaterial();
	material->SetScalarParameterValue("Percent", prctBar);

	int displayedSpeed = speed / maxSpeed * 700;
	FString str = FString::FromInt(displayedSpeed);
	SpeedText->SetText(FText::FromString(str));
}

void UGameHUD::UpdateTimer(float delta)
{
	m_elapsedTimeRace += delta;

	int min = static_cast<int>(m_elapsedTimeRace) / 60;
	int sec = static_cast<int>(m_elapsedTimeRace) % 60;
	int mil = static_cast<int>((m_elapsedTimeRace - static_cast<int>(m_elapsedTimeRace)) * 100);

	FString minStr = FString::Printf(TEXT("%02d"), min);
	FString secStr = FString::Printf(TEXT("%02d"), sec);
	FString milStr = FString::Printf(TEXT("%02d"), mil);

	FString formattedTime = minStr + ":" + secStr + ":" + milStr;

	TimerText->SetText(FText::FromString(formattedTime));
}

void UGameHUD::UpdateLap(int currentLap, int totalLaps)
{
	if (currentLap == 0) {
		currentLap = 1;
	}

	if (currentLap > totalLaps) {
		currentLap = totalLaps;
	}

	FString currentLapStr = FString::FromInt(currentLap);
	FString totalLapStr = FString::FromInt(totalLaps);
	FString formattedlaps = currentLapStr + "/" + totalLapStr;
	
	LapText->SetText(FText::FromString(formattedlaps));
}

void UGameHUD::UpdatePos()
{

}