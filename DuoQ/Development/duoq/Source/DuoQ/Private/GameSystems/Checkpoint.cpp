// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSystems/Checkpoint.h"
#include "GameSystems/DuoQGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameSystems/SaveSubSystem.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"
#include "Components/BillboardComponent.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "Components/TextRenderComponent.h"
#include "GameSystems/Dialogue/DialogueTextRenderComponent.h"
#include "GameSystems/Dialogue/DialogueTriggerComponent.h"

// Sets default values
ACheckpoint::ACheckpoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	RootComponent = OverlapVolume;

	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardComponent"));
	BillboardComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

#if WITH_EDITOR
	static ConstructorHelpers::FObjectFinder<UTexture2D> RetrievedSprite(TEXT("Engine/EditorResources/S_Actor"));
	UTexture2D*											 SpriteTexture = RetrievedSprite.Object;
	// Set the sprite texture
	if (SpriteTexture)
	{
		BillboardComponent->SetSprite(SpriteTexture);
	}
#endif
	BillboardComponent->bIsEditorOnly = true;

	DialogueTriggerComponent = CreateDefaultSubobject<UDialogueTriggerComponent>(TEXT("DialogueTriggerComponent"));

	TextRendererComponent = CreateDefaultSubobject<UDialogueTextRenderComponent>(TEXT("DialogueName"));
	TextRendererComponent->SetupAttachment(RootComponent);
	TextRendererComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 10.0f));
	TextRendererComponent->SetWorldRotation(FRotator(0.0f));
	TextRendererComponent->SetWorldScale3D(FVector(1.0f));
}

// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();
	OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ACheckpoint::OverlapBegins);
}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACheckpoint::OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (OtherActor == MyCharacter)
	{
		UDuoQGameInstance* GameInstance = Cast<UDuoQGameInstance>(GetWorld()->GetGameInstance());
		USaveSubSystem*	   SaveSystem = GameInstance->GetSubsystem<USaveSubSystem>();
		/** set the encounter for the player*/
		if (ADuoQPlayerCharacter* Player = Cast<ADuoQPlayerCharacter>(MyCharacter))
		{
			Player->CurrentEncounter = SetEncounter;
		}
		SaveSystem->Save(CheckpointProgress);

		DialogueTriggerComponent->TriggerDialogue();

		

		/* should be defunct with new dialogue trigger system
		if (TriggeredDialogue != "")
		{
			if(UDialogueSubsystem* Dialogue = GameInstance->GetSubsystem<UDialogueSubsystem>())
			{
				if(bIsCritical)
				{
					Dialogue->ActivateBundle(TriggeredDialogue);
				}
				else
				{
					Dialogue->DoCallout(TriggeredDialogue);
				}
			}
		}
		*/
		OnTriggered.Broadcast();
		Destroy();
	}
}