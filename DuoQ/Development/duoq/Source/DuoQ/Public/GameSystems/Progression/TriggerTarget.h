// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trigger.h"
#include "TriggerTarget.generated.h"

class UDialogueTriggerComponent;

UENUM(BlueprintType)
enum class ETriggerMode : uint8
{
	AND			UMETA(DisplayName = "And"),
	OR			UMETA(DisplayName = "Or"),
	COUNT		UMETA(DisplayName = "Count"),
	ANDNOT		UMETA(DisplayName = "AndNot"),
	NOTDIALOGUE UMETA(DisplayName = "NotDialogue"),
	NOTRELAY	UMETA(DisplayName = "NotRelay")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTargetTriggered);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNotRelayTriggered);

UCLASS()
class DUOQ_API ATriggerTarget : public ATrigger
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATriggerTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	TArray<TSoftObjectPtr<class ATrigger>> MyTriggers;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	TArray<TSoftObjectPtr<class ADuoQEnemyCharacter>> MyEnemyTriggers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	TArray<TSoftObjectPtr<class ADuoQCharacter>> MyAllyTriggers;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	TArray<FString> MyDialogueEventTriggers;

	UPROPERTY(EditAnywhere, Category = "Interaction", BlueprintReadOnly)
	ETriggerMode MyTriggerMode = ETriggerMode::AND;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	int Threshold = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bIsRelay = false;
	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool bIsInverseRelay = false;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool bIsSingleTrigger = true;
	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool bIsOneWay = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	int CurrentCount = 0;

	/** count the number of dialogues triggered*/
	UPROPERTY(EditAnywhere, Category = "Interaction")
	int DialogueCount = 0;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool bHasEverTriggered = false;

	UPROPERTY(EditAnywhere, Category = "Interaction")
	bool bIsSingleCountTrigger = false;

public:
	virtual void Trigger(FVector source);

	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "Event")
	FOnTargetTriggered OnTriggered;

	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "Event")
	FOnNotRelayTriggered OnNotRelayTriggered;

	TArray<FString> FinishedDialogueEventTriggers;

	UFUNCTION(BlueprintCallable)
	void Trigger();

	UFUNCTION(BlueprintCallable)
	void UnTrigger();

	UFUNCTION(BlueprintCallable)
	void HandleOnTriggerActivated();

	UFUNCTION()
	void EnemyKilled(FVector source)
	{
		UE_LOG(LogTemp, Display, TEXT("KILLED at %s"), *source.ToString());
		Trigger(source);
	};

	UFUNCTION()
	void EventTriggered(FString EventName);

	UFUNCTION()
	void EventUnTriggered(FString EventName);

	UFUNCTION()
	ETriggerMode GetTriggerMode() const { return MyTriggerMode; }

	UFUNCTION(BlueprintNativeEvent)
	void OnTrigger(FVector source);

	UFUNCTION(BlueprintNativeEvent)
	void OnUnTrigger();

	UPROPERTY(EditAnywhere, Category = "Dialogue", BlueprintReadWrite)
	TObjectPtr<UDialogueTriggerComponent> DialogueTriggerComponent;
};
