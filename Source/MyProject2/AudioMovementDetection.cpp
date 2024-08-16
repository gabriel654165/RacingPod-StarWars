#include "AudioMovementDetection.h"

UAudioMovementDetection::UAudioMovementDetection()
{
	PrimaryComponentTick.bCanEverTick = true;

	currentRawSpeed = 0;
	currentSmoothSpeed = 0;
	speedSumForAverage = 0;
	speedHistoryLength = 10;

	maxSpeed = 1000.0f;
}

void UAudioMovementDetection::BeginPlay()
{
	Super::BeginPlay();

	lastFramePosition = GetOwner()->GetActorLocation();

	//if (soundToPlay && GetOwner())
	//{
	//	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "OUI"); }
	//	UGameplayStatics::PlaySoundAtLocation(this, soundToPlay, GetOwner()->GetActorLocation());
	//}
	//else {
	//	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "NON"); }
	//}
}


void UAudioMovementDetection::Play()
{
	//if (soundToPlay && GetOwner())
	//{
	//	UGameplayStatics::PlaySoundAtLocation(this, soundToPlay, GetOwner()->GetActorLocation());
	//}
}


void UAudioMovementDetection::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	FVector currentPosition = GetOwner()->GetActorLocation();
	currentRawSpeed = FVector::Dist(currentPosition, lastFramePosition) / DeltaTime;

	speedSumForAverage += currentRawSpeed;
	speedHistory.Add(currentRawSpeed);

	if (speedHistory.Num() > speedHistoryLength)
	{
		speedSumForAverage -= speedHistory[0];
		speedHistory.RemoveAt(0);
	}

	currentSmoothSpeed = speedSumForAverage / FMath::Min(speedHistoryLength, speedHistory.Num());
	currentRawSpeed = FMath::Clamp(currentSmoothSpeed / maxSpeed, 0.0f, 1.0f);

	//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("RAW %f"), currentRawSpeed)); }
	//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("SMOOTH %f"), currentSmoothSpeed)); }

	lastFramePosition = currentPosition;
}

