// Copyright 2024


#include "GameSystems/EnemyLoader.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameSystems/DuoQSaveSystem.h"
#include "GameSystems/SaveSubSystem.h"

// Sets default values
AEnemyLoader::AEnemyLoader()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	RootComponent = OverlapVolume;
	OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &AEnemyLoader::OverlapBegins);

}

// Called when the game starts or when spawned
void AEnemyLoader::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyLoader::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyLoader::OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (OtherActor == MyCharacter && LevelToLoad != "")
	{

		if (UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
		{
			//if (!SaveGameInstance->EnemyStatus.Find(LevelToLoad))
			{
				FLatentActionInfo LatentInfo;
				UGameplayStatics::LoadStreamLevel(this, LevelToLoad, true, true, LatentInfo);
			}
		}
	}
}

