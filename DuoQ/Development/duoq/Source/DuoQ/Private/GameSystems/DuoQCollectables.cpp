// Copyright 2024

#include "GameSystems/DuoQCollectables.h"
#include "GameSystems/DuoQSaveSystem.h"
#include "GameSystems/SaveSubSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameSystems/CollectableSubsystem.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "GameSystems/Dialogue/DialogueTextRenderComponent.h"
#include "GameSystems/Progression/ColllisionTrigger.h"
#include "GameSystems/Dialogue/DialogueTriggerComponent.h"

// Sets default values
ADuoQCollectables::ADuoQCollectables()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh3"));
	StaticMesh->SetupAttachment(RootComponent);
	//  RootComponent = StaticMesh;
	TextRenderComponent = CreateDefaultSubobject<UDialogueTextRenderComponent>(TEXT("TextRenderer"));
	TextRenderComponent->SetupAttachment(RootComponent);
	DialogueTriggerComponent = CreateDefaultSubobject<UDialogueTriggerComponent>(TEXT("DialogueTriggerComponent"));

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ADuoQCollectables::BeginPlay()
{
	Super::BeginPlay();

	if (UDuoQSaveSystem* SaveGameInstance = Cast<UDuoQSaveSystem>(UGameplayStatics::LoadGameFromSlot("TestSaveSlot", 0)))
	{
		FName Name = FName(GetName());
		if (SaveGameInstance->SavedCollectables.Contains(Name))
		{
			//Destroy();
			//return;
		}
	}
	// add this actor to the list of Collectables to Collectable Subsystem
	if (UDuoQGameInstance* DGI = UDuoQBlueprintFunctionLibrary::GetDuoQGameInstance(this))
	{
		// trying to grab the collectable subsystem
		if (UCollectableSubsystem* CS = DGI->GetSubsystem<UCollectableSubsystem>())
		{
			CS->AddToCollectables(this);
		}
	}
}

void ADuoQCollectables::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// remove this actor from the list of Collectables to Collectable Subsystem
	if (UDuoQGameInstance* DGI = UDuoQBlueprintFunctionLibrary::GetDuoQGameInstance(this))
	{
		// trying to grab the collectable subsystem
		if (UCollectableSubsystem* CS = DGI->GetSubsystem<UCollectableSubsystem>())
		{
			CS->RemoveFromCollectables(this);
		}
	}
}

// Called every frame
void ADuoQCollectables::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FString ADuoQCollectables::GetCollectableName()
{
	return GetName();
}
