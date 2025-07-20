// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DuoQCollectables.generated.h"

UCLASS()
class DUOQ_API ADuoQCollectables : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADuoQCollectables();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCollected = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UDialogueTextRenderComponent> TextRenderComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UDialogueTriggerComponent> DialogueTriggerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> StaticMesh;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	FString GetCollectableName();
};
