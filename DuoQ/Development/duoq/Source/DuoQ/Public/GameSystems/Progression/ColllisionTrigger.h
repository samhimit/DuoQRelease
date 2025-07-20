// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystems/Progression/TriggerTarget.h"
#include "ColllisionTrigger.generated.h"

class UDialogueTextRenderComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCollisionTriggered);

/**
 * Base class for DuoQ Triggers and Progression
 */
UCLASS()
class DUOQ_API ACollisionTrigger : public ATriggerTarget
{
	GENERATED_BODY()
	
public:
	// Overlap volume to trigger level streaming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* OverlapVolume;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBillboardComponent* BillboardComponent;
	ACollisionTrigger();
	UFUNCTION()
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsDestroyOnUse = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsTriggeredByPlayer = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsTriggeredByStarlight = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsDeactivatable = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsReversed = false;

	/** if we should autimatically untrigger the thing if we left the area*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bShouldUnTriggerIfLeave = true;

	bool bIsCollisionActivated = false;

	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FOnCollisionTriggered OnCollision;
	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "Event")
	FOnTargetTriggered OnTriggeredAndCollided;
	UPROPERTY(EditAnywhere)
	int bNumTriggered = 0;
	UFUNCTION()
	void OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OverlapEnds(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void ActivateTrigger(FVector source) override;

	void SendSignal(bool bActivated);

	virtual void OnTrigger_Implementation(FVector source) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDialogueTextRenderComponent> TextRendererComponent;
};
