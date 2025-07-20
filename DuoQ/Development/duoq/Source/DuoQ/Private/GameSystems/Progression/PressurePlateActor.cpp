// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSystems/Progression/PressurePlateActor.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

// Sets default values
APressurePlateActor::APressurePlateActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// creating the box collision component
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Box->InitBoxExtent(FVector(150.0f,150.0f,50.0f));
}

// Called when the game starts or when spawned
void APressurePlateActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APressurePlateActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
