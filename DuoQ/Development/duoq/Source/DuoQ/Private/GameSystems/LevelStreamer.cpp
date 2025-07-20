// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/LevelStreamer.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"


// Sets default values
ALevelStreamer::ALevelStreamer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	RootComponent = OverlapVolume;
	OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ALevelStreamer::OverlapBegins);
	OverlapVolume->OnComponentEndOverlap.AddUniqueDynamic(this, &ALevelStreamer::OverlapEnds);
}

// Called when the game starts or when spawned
void ALevelStreamer::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> Result;
	GetOverlappingActors(Result);
	for(AActor * actor : Result)
	{
		ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (actor == MyCharacter && LevelToLoad != "")
		{
			FLatentActionInfo LatentInfo;
			UGameplayStatics::LoadStreamLevel(this, LevelToLoad, true, true, LatentInfo);
		}
	}
}

// Called every frame
void ALevelStreamer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ALevelStreamer::OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (OtherActor == MyCharacter && LevelToLoad != "")
	{
		FLatentActionInfo LatentInfo;
		UGameplayStatics::LoadStreamLevel(this, LevelToLoad, true, true, LatentInfo);
		OnTriggered.Broadcast();
	}
}
void ALevelStreamer::OverlapEnds(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (OtherActor == MyCharacter && LevelToLoad != "")
	{
		FLatentActionInfo LatentInfo;
		UGameplayStatics::UnloadStreamLevel(this, LevelToLoad, LatentInfo, true);
	}
}

