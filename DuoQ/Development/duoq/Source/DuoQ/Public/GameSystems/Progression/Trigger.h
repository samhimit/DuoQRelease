// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trigger.generated.h"

UCLASS()
class DUOQ_API ATrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray <TSoftObjectPtr<class ATriggerTarget>> MyTargets;

public:	
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float DelayTime = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool bNeedOnDeactivated = true;
	
	FTimerHandle MyTimer;
	FTimerDelegate MyTimerDelegate;
	
	void		 AddTarget(TSoftObjectPtr<class ATriggerTarget> target) { MyTargets.Add(target); };
	
	UFUNCTION(BlueprintCallable)
	virtual void		 ActivateTrigger(FVector source);
	
	void ActivateTrigger();
	
	UFUNCTION(BlueprintCallable)
	void DeactivateTrigger();

	UFUNCTION(BlueprintCallable)
	void DoTriggerWithSource(FVector source);

	
	UFUNCTION(BlueprintImplementableEvent)
	void OnActivated(FVector source);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnDeactivated();

};
