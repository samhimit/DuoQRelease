// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PressurePlateActor.generated.h"

UCLASS()
class DUOQ_API APressurePlateActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APressurePlateActor();

protected:

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	class UBoxComponent* Box = nullptr;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
