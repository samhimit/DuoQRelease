// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "Checkpoint.generated.h"

class UDialogueTextRenderComponent;
class UTextRenderComponent;
class UDialogueTriggerComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCheckpointTriggered);

UCLASS()
class DUOQ_API ACheckpoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACheckpoint();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** determines which encounter the player is in*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPlayerEncounter SetEncounter = EPlayerEncounter::UnShieldedGrunt;

	UFUNCTION()
	void OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	int CheckpointProgress;
	// Overlap volume to trigger level streaming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* OverlapVolume;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UBillboardComponent* BillboardComponent;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCheckpointTriggered OnTriggered;

	UPROPERTY(EditAnywhere, Category = "Dialogue", BlueprintReadWrite)
	TObjectPtr<UDialogueTriggerComponent> DialogueTriggerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDialogueTextRenderComponent> TextRendererComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};