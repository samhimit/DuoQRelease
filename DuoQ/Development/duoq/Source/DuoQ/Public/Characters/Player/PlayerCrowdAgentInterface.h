// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Navigation/CrowdManager.h"
#include "Navigation/CrowdAgentInterface.h"
#include "PlayerCrowdAgentInterface.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUOQ_API UPlayerCrowdAgentInterface : public UActorComponent, public ICrowdAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerCrowdAgentInterface();

	FVector GetCrowdAgentLocation() const;

	FVector GetCrowdAgentVelocity() const;

	void GetCrowdAgentCollisions(float& CylinderRadius, float& CylinderHalfHeight) const;

	float GetCrowdAgentMaxSpeed() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void InitializeComponent();
};
