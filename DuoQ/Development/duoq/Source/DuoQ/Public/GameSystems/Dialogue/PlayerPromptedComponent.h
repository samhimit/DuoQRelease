// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerPromptedComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent),Blueprintable )
class DUOQ_API UPlayerPromptedComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerPromptedComponent();

	UPROPERTY(EditInstanceOnly)
	TMap<FString, FString> Prompts;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly)
	bool bAddPrompts = true;

	UFUNCTION()
	void UpdatePrompts();
};
