#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "GameFramework/Actor.h"
#include "DialogueTrigger.generated.h"

UCLASS()
class DUOQ_API ADialogueTrigger : public ATriggerBox
{
	GENERATED_BODY()

public:
	ADialogueTrigger();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	FString BundleName;
	
	UFUNCTION()
	void OnPlayerEnterTriggerBox(AActor* OverlappedActor, AActor* OtherActor);
};
