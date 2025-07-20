// Copyright 2024


#include "GameSystems/Progression/CreditsSubsystem.h"

#include "CreditsWidget.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "MainMenuGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"
#include "GameSystems/Dialogue/StarlightResponseSubsystem.h"
#include "GameSystems/Integrations/DuoQDiscordSubsystem.h"
#include "IntroSequence/MainMenuPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/OutputDeviceNull.h"
#include "UI/GameplayHUD.h"
#include "UI/OffscreenIndicatorWidget.h"

void UCreditsSubsystem::StartCreditsSequence(FString EventName)
{
	bIsPlayingCredits = true;
	
	if(UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		DialogueSubsystem->OnDialogueEnd.RemoveAll(this);
	}

	FTimerHandle QuitTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(QuitTimerHandle, [this](){
		
		if(UDuoQHUD* DuoQHUD = UDuoQBlueprintFunctionLibrary::GetHUDInstance(this))
		{
			DuoQHUD->GameplayHUD->TalaLeaveGame();
		}

		if(ADuoQPlayerController* Controller = UDuoQBlueprintFunctionLibrary::GetDuoQPlayerController(this,0))
		{
			Controller->GetOffscreenIndicatorWidget()->bShouldBeHiddenPermanently = true;
		}

		if(AStarlightCharacter* StarlightCharacter = UDuoQBlueprintFunctionLibrary::GetStarlightCharacter(this))
		{
			StarlightCharacter->Destroy();
		}

		if(UDuoQDiscordSubsystem* DuoQDiscordSubsystem = GEngine->GetEngineSubsystem<UDuoQDiscordSubsystem>())
		{
			DuoQDiscordSubsystem->SetLobbyText(TEXT("To The Heart"),TEXT("In Game (1/2)"));
		}

		if(ADuoQPlayerCharacter* PlayerCharacter = UDuoQBlueprintFunctionLibrary::GetDuoQPlayer(this))
		{
			USceneCaptureComponent2D* Camera = PlayerCharacter->GetSceneCaptureComponent();
			AsyncTask(ENamedThreads::GameThread, [this, Camera]()
				{
					// "Camera" = 2D Scene Capture Component
					if (Camera)
					{
						Camera->SetVisibility(true);
										// define image resolution
						Camera->TextureTarget = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(this, UCanvasRenderTarget2D::StaticClass(), 1920, 1080);
						Camera->CaptureScene();
						// Creates Texture2D to store TextureRenderTarget content
						Texture = UTexture2D::CreateTransient(Camera->TextureTarget->SizeX, Camera->TextureTarget->SizeY);
		#if WITH_EDITORONLY_DATA
						Texture->MipGenSettings = TMGS_NoMipmaps;
		#endif
						Texture->SRGB = true;
						Texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;

						// Read the pixels from the RenderTarget and store them in a FColor array
						TArray<FColor> SurfData;
						FRenderTarget* RenderTarget = Camera->TextureTarget->GameThread_GetRenderTargetResource();
						FReadSurfaceDataFlags Flags = FReadSurfaceDataFlags(RCM_MinMax, CubeFace_MAX);
						Flags.SetLinearToGamma(false);
						RenderTarget->ReadPixels(SurfData);
						// some alpha channels are set to 0 by default, make sure all pixels opaque
						for (auto& Pixel : SurfData)
						{
							Pixel.A = 255.f;
						}

						// Lock and copies the data between the textures
						void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
						const int32 TextureDataSize = SurfData.Num() * 4;
						FMemory::Memcpy(TextureData, SurfData.GetData(), TextureDataSize);
						Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
						// Apply Texture changes to GPU memory
						Texture->UpdateResource();
						// Do something with texture
					}
				});
		}

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UCreditsSubsystem::FadeOutFromGameplay, 5.0f, false); // Adjust the delay as needed
		
	}, 1.0f, false); // Adjust the delay as needed
}

void UCreditsSubsystem::StartCreditsFromMenu(FString EventName)
{
	if(UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		DialogueSubsystem->OnDialogueEnd.RemoveAll(this);
	}
	DisplayCreditsOnMonitor();
	PlayCredits();
}

void UCreditsSubsystem::FadeOutFromGameplay()
{
	UE_LOG(LogTemp, Warning, TEXT("ShowCreditsScreen function called"));

	// Fade to black
	if (FadingScreenClass)
	{
		FadingScreenInstance = CreateWidget<UUserWidget>(GetWorld(), FadingScreenClass);

		if (FadingScreenInstance)
		{
			FadingScreenInstance->AddToViewport();

			FOutputDeviceNull OutputDevice;
			FadingScreenInstance->CallFunctionByNameWithArguments(TEXT("StartFadeAnimation"), OutputDevice, nullptr, true);

			// Hides the dialogue before loading the MainMenu level so that loading a new level won't crash the HideDialogue function
			if (UStarlightResponseSubsystem* StarlightResponseSubsystem = GetGameInstance()->GetSubsystem<UStarlightResponseSubsystem>())
			{
				StarlightResponseSubsystem->HideDialogue();
			}

			GetWorld()->GetTimerManager().SetTimer(FadeOutTimerHandle, this, &UCreditsSubsystem::TransitionToMenu, 3.0f, false);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FadingScreenClass is not assigned!"));
	}
}

void UCreditsSubsystem::TransitionToMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("ShowCreditsAfterFade function called"));

	if(UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		DialogueSubsystem->bBlockDialogue = false;
	}

	// Load the MainMenu level
	UGameplayStatics::OpenLevel(this, FName("MainMenu"));
}

void UCreditsSubsystem::DisplayCreditsOnMonitor()
{
	if (AMainMenuGameMode* MenuGameMode = Cast<AMainMenuGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		CreditsScreenInstance = CreateWidget<UCreditsWidget>(GetWorld(), CreditsScreenClass);

		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

		if (CreditsScreenInstance)
		{
			MenuGameMode->MonitorWidget->SetWidget(CreditsScreenInstance);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MenuGameMode not found in the world, failed to display credits on the monitor"));
	}
}

void UCreditsSubsystem::PlayCredits()
{
	if(CreditsScreenInstance)
	{
		CreditsScreenInstance->StartCredits();
	}
}

void UCreditsSubsystem::PlayEndGameCameraSequence()
{
	// Call the PlayEndGameSequence event in MainMenu_BP
	AActor* MainMenuActor = UGameplayStatics::GetActorOfClass(GetWorld(), AMainMenuGameMode::StaticClass());
	if (MainMenuActor)
	{
		UFunction* PlayEndGameSequenceFunction = MainMenuActor->FindFunction(TEXT("PlayEndGameSequence"));
		if (PlayEndGameSequenceFunction)
		{
			MainMenuActor->ProcessEvent(PlayEndGameSequenceFunction, nullptr);
		}
	}

	if (AMainMenuGameMode* MenuGameMode = Cast<AMainMenuGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		MenuGameMode->MonitorWidget->SetWidget(nullptr);
	}

	if(UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		DialogueSubsystem->bBlockDialogue = false;
	}

	bIsPlayingCredits = false;
}