// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TheUltimateDialogueGrabber.generated.h"

UCLASS()
class DUOQEDITOR_API ATheUltimateDialogueGrabber : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATheUltimateDialogueGrabber();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditDefaultsOnly)
	UBlueprint* BlueprintSubsystem;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* DataTable;

	UFUNCTION(CallInEditor)
	void GrabDialogue();

};
