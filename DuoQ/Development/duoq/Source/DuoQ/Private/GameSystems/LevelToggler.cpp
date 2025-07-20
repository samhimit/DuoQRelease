// Copyright 2024


#include "GameSystems/LevelToggler.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Characters/Player/DuoQPlayerCharacter.h"

// Sets default values
ALevelToggler::ALevelToggler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALevelToggler::BeginPlay()
{
	Super::BeginPlay();
	
}
FName ALevelToggler::AppendName(FName name, FString string)
{
	FString base = name.ToString();
	base += string;

	return FName(*base);
}

void ALevelToggler::OnUnTrigger_Implementation()
{
	Super::OnUnTrigger_Implementation();
	GetWorld()->GetTimerManager().SetTimer(
		timer,
		[this]() {
			for (FName name : LevelsToLoad)
			{
				LoadSingleLevel(name, FString("_Effects"));
				LoadSingleLevel(name, FString("_Props"));
				LoadSingleLevel(name, FString("_Progression"));
				LoadSingleLevel(name, FString("_Geometry"));
			}
			for (FName name : LevelsToUnload)
			{
				UnloadSingleLevel(name, FString("_Effects"));
				UnloadSingleLevel(name, FString("_Props"));
				UnloadSingleLevel(name, FString("_Progression"));
				UnloadSingleLevel(name, FString("_Geometry"));
			}
			OnToggled.Broadcast();
			//ActivateTrigger();
		},
		5.0f,
		false, 5.0f);
}
void ALevelToggler::LoadSingleLevel(FName name, FString str)
{
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = "CompleteTask";
	LatentInfo.UUID = NumTasks;
	LatentInfo.Linkage = 0;
	UGameplayStatics::LoadStreamLevel(this, AppendName(name, str), true, true, LatentInfo);
	NumTasks++;
}
void ALevelToggler::UnloadSingleLevel(FName name, FString str)
{
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = "CompleteTask";
	LatentInfo.UUID = NumTasks;
	LatentInfo.Linkage = 0;
	UGameplayStatics::UnloadStreamLevel(this, AppendName(name, str), LatentInfo, true);
	NumTasks++;
}

// Called every frame
void ALevelToggler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelToggler::CompleteTask()
{
	NumCompleted++;
	if (NumCompleted >= NumTasks)
	{
		ActivateTrigger();
	}
}