// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Characters/Partner/StarlightGOAPController.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"

#include "StarlightReactionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUOQ_API UStarlightReactionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStarlightReactionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShootingDuration = 2.0f;


private:
	UPROPERTY()
	AStarlightGOAPController* StarlightAIController;

	UPROPERTY()
	UAIPerceptionComponent* PerceptionComponent;

	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	FTimerHandle ShootingTimerHandle;
	void OnStopShooting() const;
};
