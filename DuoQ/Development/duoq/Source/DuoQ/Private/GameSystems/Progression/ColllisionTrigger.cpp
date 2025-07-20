// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSystems/Progression/ColllisionTrigger.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"
#include "GameSystems/Dialogue/DialogueTextRenderComponent.h"
#include "GameSystems/Dialogue/DialogueTriggerComponent.h"

ACollisionTrigger::ACollisionTrigger()
{

	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	RootComponent = OverlapVolume;
	OverlapVolume->SetCanEverAffectNavigation(false);
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

	bIsRelay = true;

	TextRendererComponent = CreateDefaultSubobject<UDialogueTextRenderComponent>(TEXT("DialogueName"));
	TextRendererComponent->SetupAttachment(RootComponent);
	TextRendererComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 10.0f));
	TextRendererComponent->SetWorldRotation(FRotator(0.0f));
	TextRendererComponent->SetWorldScale3D(FVector(1.0f));
}

void ACollisionTrigger::BeginPlay()
{
	Super::BeginPlay();
	OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ACollisionTrigger::OverlapBegins);
	OverlapVolume->OnComponentEndOverlap.AddUniqueDynamic(this, &ACollisionTrigger::OverlapEnds);
}

void ACollisionTrigger::OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	ACharacter* StarlightCharacter = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(GetWorld());
	if ((bIsTriggeredByPlayer && OtherActor == MyCharacter) || (bIsTriggeredByStarlight && OtherActor == StarlightCharacter))
	{
		if (bNumTriggered == 0)
		{

			OnCollision.Broadcast();
			if (bIsReversed)
			{
				// the pressure plate is now off bc we entered it
				SendSignal(false);
			}
			else
			{
				// pressure plate *could* now be on bc we stepped on it
				SendSignal(true);

				/* should be defunct bc of new dialoguetrigger componenets
				if (TriggeredDialogue != "")
				{
					if (bDialogueSingleTrigger || !bHasTriggered)
					{
						if (UDialogueSubsystem* Dialogue = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
						{
							bHasTriggered = true;
							if (bIsCritical)
							{
								Dialogue->ActivateBundle(TriggeredDialogue);
							}
							else
							{
								Dialogue->DoCallout(TriggeredDialogue);
							}
						}
					}
				}
				*/
			}
		}
		bNumTriggered++;

	}
}
void ACollisionTrigger::OverlapEnds(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bIsDeactivatable)
		return;
	ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	ACharacter* StarlightCharacter = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(GetWorld());
	if ((bIsTriggeredByPlayer && OtherActor == MyCharacter) || (bIsTriggeredByStarlight && OtherActor == StarlightCharacter))
	{
		bNumTriggered--;
		if (bNumTriggered > 0)
			return;
		if (bIsReversed)
		{
			// same logic as above it *could* be on now
			SendSignal(true);
		}
		else
		{
			// pressure plate is off now
			SendSignal(false);
		}

		// you can't have negative things in a box
		if(bNumTriggered < 0)
		{
			bNumTriggered = 0;
		}
	}
}

void ACollisionTrigger::ActivateTrigger(FVector source)
{
	// since this function is called from the prereqs, we only want to relay it to the next target (technically should have isRelay)
	if (bIsCollisionActivated)
	{
		Super::ActivateTrigger(source);
		OnTriggeredAndCollided.Broadcast();
		DialogueTriggerComponent->TriggerDialogue();
		if (bIsDestroyOnUse)
		{
			Destroy();
		}
	}
}

void ACollisionTrigger::SendSignal(bool bActivated)
{
	bIsCollisionActivated = bActivated;
	// if the pressure plate is activated AND the prereqs are met we can activate
	if (CurrentCount == Threshold && bActivated)
	{
		ActivateTrigger(GetActorLocation());
	}
	// any time the pressure plate is stepped off (regardless of prereqs) we should deactivate
	else if (!bActivated)
	{
		if (bShouldUnTriggerIfLeave)
		{
			DeactivateTrigger();
		}
	}
}

void ACollisionTrigger::OnTrigger_Implementation(FVector source)
{
	if (bIsRelay)
		ActivateTrigger(source);
}