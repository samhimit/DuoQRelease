#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "Characters/Partner/StarlightCharacter.h"
#include "Characters/HealthComponent.h"
#include "Amicus.generated.h"

// Forward Declaration
class AAmicusWeakPoint;

UCLASS()
class DUOQ_API AAmicus : public AActor
{
	GENERATED_BODY()

public:
	// Constructor
	AAmicus();

	UFUNCTION(BlueprintCallable, Category = "Weak Points")
	void LightUpWeakPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Amicus | Timer")
	float MaxTimeTillDeath;

private:
	FTimerHandle TimerHandle;
	// Array to store all weak points found in the level
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weak Points", meta = (AllowPrivateAccess = "true"))
	TArray<AAmicusWeakPoint*> WeakPoints;

	// Array to store all correct weak points
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weak Points", meta = (AllowPrivateAccess = "true"))
	TArray<AAmicusWeakPoint*> CorrectWeakPoints;

	// Array to store the correct weak points colors
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weak Points", meta = (AllowPrivateAccess = "true"))
	TArray<FLinearColor> CorrectWeakPointColors;

	void FindAllWeakPoints();
	void SelectCorrectWeakPoints();
	void FindPlayerAndStarlight();
	void StartTimer();
	void TimerExpired();
	void UnlitWeakPoints();

	void OnCorrectWeakPointDestroyed();
	void OnWrongWeakPointDestroyed();

	UFUNCTION()
	void OnWeakPointDestroyed(FLinearColor DestroyedWeakPointColor);

	UPROPERTY()
	ADuoQPlayerCharacter* PlayerRef;

	UPROPERTY()
	AStarlightCharacter* StarlightRef;

	int32 CorrectWeakPointsDestroyed;
	int32 EncounterNumber = 0;

	FTimerHandle LightUpTimerHandle;
	FTimerHandle Unlight;
};
