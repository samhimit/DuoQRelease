// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSystems/Communication/CommunicationSubsystem.h"

#include <string>

#include "RuntimeAudioImporter/Public/Sound/CapturableSoundWave.h"
#include "CoreGlobals.h"
#include "DuoQBlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "VaRestSubsystem.h"
#include "Characters/Partner/PriorityGOAPAction.h"
#include "Characters/Partner/StarlightCharacter.h"
#include "Engine/GameEngine.h"
#include "GameSystems/DuoQGameInstance.h"
#include "GameSystems/DuoQGameMode.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "Characters/Partner/StarlightCharacter.h"
#include "Characters/Partner/StarlightGOAPController.h"
#include "GOAPPlanner.h"
#include "NNE.h"
#include "NNEModelData.h"
#include "NNERuntimeCPU.h"
#include "GameSystems/Dialogue/StarlightResponseSubsystem.h"
#include "GameSystems/Communication/ChatGPTLLMRequest.h"
#include "GameSystems/Dialogue/DialogueSubsystem.h"
#include "GameSystems/Audio/AudioSubsystem.h"

DEFINE_LOG_CATEGORY(LogCommunication);

void UCommunicationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Start Audio Recording
	SpeechRecognizer = USpeechRecognizer::CreateSpeechRecognizer();
	SpeechRecognizer->OnRecognitionFinished.AddDynamic(this, &UCommunicationSubsystem::OnRecognitionEnd);
	SpeechRecognizer->OnRecognitionError.AddDynamic(this, &UCommunicationSubsystem::OnRecognitionError);
	SpeechRecognizer->OnRecognizedTextSegment.AddDynamic(this, &UCommunicationSubsystem::OnRecognizedText);
	SpeechRecognizer->OnRecognitionProgress.AddDynamic(this, &UCommunicationSubsystem::OnRecognitionProgress);

	SpeechRecognizer->SetStreamingDefaults();
	// let's try some different settings to break things down into small chunks
	SpeechRecognizer->SetStepSize(SpeechStepSize);
	SpeechRecognizer->SetSuppressNonSpeechTokens(true);
	SpeechRecognizer->SetLanguage(ESpeechRecognizerLanguage::En);

	OnSpeechRecognitionStartedDynamic.BindDynamic(this, &UCommunicationSubsystem::OnStartSpeechRecognition);
	SpeechRecognizer->StartSpeechRecognition(OnSpeechRecognitionStartedDynamic);

	DeviceResultDelegate.BindDynamic(this, &UCommunicationSubsystem::UpdateInputDeviceSampleRate);

	// initialize VAD
	/*TWeakInterfacePtr<INNERuntimeCPU> Runtime = UE::NNE::GetRuntime<INNERuntimeCPU>(FString("NNERuntimeIREECpu"));
	if (Runtime.IsValid())
	{
		if(Runtime->CanCreateModelCPU(PreLoadedModelData) == UE::NNE::EResultStatus::Ok)
		{
			TSharedPtr<UE::NNE::IModelCPU> Model = Runtime->CreateModelCPU(PreLoadedModelData);
			if (Model.IsValid())
			{
				ModelInstance = Model->CreateModelInstanceCPU();
				if (ModelInstance.IsValid())
				{
					const TArray<float> Empty = {0};
					SetUpModelTensorBindings(Empty);
				}
				else
				{
					UE_LOG(LogCommunication, Error, TEXT("Failed to create the model instance"));
				}
			}
			else
			{
				UE_LOG(LogCommunication, Error, TEXT("Failed to create the model"));
			}
		}
	}
	else
	{
		UE_LOG(LogCommunication, Error, TEXT("Cannot find runtime NNERuntimeIREECpu, please enable the corresponding plugin"));
	}*/
}

void UCommunicationSubsystem::Deinitialize()
{
	Super::Deinitialize();
	if (CapturableSoundWave)
		CapturableSoundWave->StopCapture();
}
/*
void UCommunicationSubsystem::SetUpModelTensorBindings(const TArray<float>& PopulatedAudioData)
{
   InputTensorShapes.Empty();
   OutputTensorShapes.Empty();

   // querying and testing in- and outputs
   TConstArrayView<UE::NNE::FTensorDesc> InputTensorDescs = ModelInstance->GetInputTensorDescs();
   // creating input tensor shapes
   {
	   // Input
	   {
		   TArray<int32> SymbolicTensorShapeDimensionsArray = {1,PopulatedAudioData.Num()};
		   TConstArrayView<int32> NewSymbolicTensorShapeDimensions(SymbolicTensorShapeDimensionsArray);
		   auto NewSymbolicInputTensorShape = UE::NNE::FSymbolicTensorShape::Make(NewSymbolicTensorShapeDimensions);
		   InputTensorShapes.Emplace(UE::NNE::FTensorShape::MakeFromSymbolic(NewSymbolicInputTensorShape));
	   }
	   // state
	   {
		   TArray<int32> SymbolicTensorShapeDimensionsArray = {2,1,128};
		   TConstArrayView<int32> NewSymbolicTensorShapeDimensions(SymbolicTensorShapeDimensionsArray);
		   auto NewSymbolicInputTensorShape = UE::NNE::FSymbolicTensorShape::Make(NewSymbolicTensorShapeDimensions);
		   InputTensorShapes.Emplace(UE::NNE::FTensorShape::MakeFromSymbolic(NewSymbolicInputTensorShape));
	   }
	   // SR
	   {
		   UE::NNE::FSymbolicTensorShape SymbolicInputTensorShape = InputTensorDescs[2].GetShape();
		   InputTensorShapes.Emplace(UE::NNE::FTensorShape::MakeFromSymbolic(SymbolicInputTensorShape));
	   }
   }

   TConstArrayView<UE::NNE::FTensorDesc> OutputTensorDescs = ModelInstance->GetOutputTensorDescs();
   {
	   // output
	   {
		   TArray<int32> SymbolicTensorShapeDimensionsArray = {1,1};
		   TConstArrayView<int32> NewSymbolicTensorShapeDimensions(SymbolicTensorShapeDimensionsArray);
		   auto NewSymbolicInputTensorShape = UE::NNE::FSymbolicTensorShape::Make(NewSymbolicTensorShapeDimensions);
		   OutputTensorShapes.Emplace(UE::NNE::FTensorShape::MakeFromSymbolic(NewSymbolicInputTensorShape));
	   }
	   // stateN
	   {
		   TArray<int32> SymbolicTensorShapeDimensionsArray = {2,1,128};
		   TConstArrayView<int32> NewSymbolicTensorShapeDimensions(SymbolicTensorShapeDimensionsArray);
		   auto NewSymbolicInputTensorShape = UE::NNE::FSymbolicTensorShape::Make(NewSymbolicTensorShapeDimensions);
		   OutputTensorShapes.Emplace(UE::NNE::FTensorShape::MakeFromSymbolic(NewSymbolicInputTensorShape));
	   }
   }


   // Prepare input data and output data here
   InputInputData = TArray<float>(PopulatedAudioData);

   if(InputStateData.IsEmpty())
   {
	   InputStateData.SetNumZeroed(InputTensorShapes[1].Volume());
   }
   InputSRData.SetNumZeroed(InputTensorShapes[2].Volume());
   InputSRData[0] = SampleRate;

   InputBindings.SetNumZeroed(3);
   InputBindings[0].Data = InputInputData.GetData();
   InputBindings[0].SizeInBytes = InputInputData.Num() * sizeof(float);
   InputBindings[1].Data = InputStateData.GetData();
   InputBindings[1].SizeInBytes = InputStateData.Num() * sizeof(float);
   InputBindings[2].Data = InputSRData.GetData();
   InputBindings[2].SizeInBytes = InputSRData.Num() * sizeof(int64);

   ModelInstance->SetInputTensorShapes(InputTensorShapes);

   OutputOutputData.SetNumZeroed(OutputTensorShapes[0].Volume());
   OutputStateNData.SetNumZeroed(OutputTensorShapes[1].Volume());

   OutputBindings.SetNumZeroed(2);
   OutputBindings[0].Data = OutputOutputData.GetData();
   OutputBindings[0].SizeInBytes = OutputOutputData.Num() * sizeof(float);
   OutputBindings[1].Data = OutputStateNData.GetData();
   OutputBindings[1].SizeInBytes = OutputStateNData.Num() * sizeof(float);
}
*/

void UCommunicationSubsystem::SetGOAPPlanner()
{
	// getting a reference to starlight
	AActor* Act = UGameplayStatics::GetActorOfClass(GetWorld(), AStarlightCharacter::StaticClass());
	if (AStarlightCharacter* Starlight = Cast<AStarlightCharacter>(Act))
	{
		AController* controller = Starlight->GetController();
		// getting starlight's controller
		if (AStarlightGOAPController* StarlightController = Cast<AStarlightGOAPController>(controller))
		{
			Planner = StarlightController->GetGOAPPlanner();
		}
	}
}

void UCommunicationSubsystem::PauseAudioCapture()
{
	if (UAudioSubsystem* AudioSubsystem = GetGameInstance()->GetSubsystem<UAudioSubsystem>())
	{
		AudioSubsystem->InputUnduckVolume();
	}
	OnToggleMute.Broadcast(true);
	// UE_LOG(LogTemp, Display, TEXT("ABBY - Pause audio capture"));
	GetWorld()->GetTimerManager().SetTimer(InputBufferTimer, this, &UCommunicationSubsystem::FinishAudioRecording, CurrentInputSampleRate > LowQualitySampleRateThreshold ? AudioInputHangTime : LowQualityAudioInputHangTime);
}

void UCommunicationSubsystem::ResumeAudioCapture()
{
	if (bUseFlushing)
	{
		bIsFlusing = false;
	}

	CapturableSoundWave->ToggleMute(false);
	BufferedWords = TEXT("");
	if (UAudioSubsystem* AudioSubsystem = GetGameInstance()->GetSubsystem<UAudioSubsystem>())
	{
		AudioSubsystem->InputDuckVolume();
	}
	OnToggleMute.Broadcast(false);
}

bool UCommunicationSubsystem::IsRecording()
{
	return CapturableSoundWave->IsCapturing();
}

void UCommunicationSubsystem::SetAudioDeviceID(signed int AudioDeviceID)
{
	SelectedAudioDeviceID = AudioDeviceID;
	// reset the audio wave so that it now uses the new device
	if(CapturableSoundWave)
	{
		CapturableSoundWave->ToggleMute(true);
		CapturableSoundWave->StopCapture();
		CapturableSoundWave->StartCapture(AudioDeviceID);
		CapturableSoundWave->ToggleMute(true);
		// CapturableSoundWave->StopCapture();

		CapturableSoundWave->GetAvailableAudioInputDevices(DeviceResultDelegate);
	}
}

signed int UCommunicationSubsystem::GetAudioDeviceID()
{
	return SelectedAudioDeviceID;
}

void UCommunicationSubsystem::PlaybackRecording()
{
	CapturableSoundWave->StopCapture();
	UGameplayStatics::PlaySound2D(this, CapturableSoundWave);
}

void UCommunicationSubsystem::AddBackupInput(FString Input)
{
	FScopeLock Lock(&BackupCommandsGuard);
	BackupCommands.Add(Input);
}

void UCommunicationSubsystem::RemoveBackupInput(FString Input)
{
	FScopeLock Lock(&BackupCommandsGuard);
	BackupCommands.Remove(Input);
}

void UCommunicationSubsystem::UpdateInputDeviceSampleRate(const TArray<FRuntimeAudioInputDeviceInfo>& AvailableDevices)
{
	if (AvailableDevices.IsValidIndex(SelectedAudioDeviceID))
	{
		CurrentInputSampleRate = AvailableDevices[SelectedAudioDeviceID].PreferredSampleRate;
	}
}

// creates the input, this is not updated for the GOAP input yet
FString UCommunicationSubsystem::GenerateInputString(const FString& PlayerInput)
{
	// add players text
	FString Output = TEXT("\"") + PlayerInput + TEXT("\"");

	if (ADuoQGameMode* DuoQGameMode = Cast<ADuoQGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (DuoQGameMode->Starlight)
		{
			if (AStarlightGOAPController* StarlightGoapController = Cast<AStarlightGOAPController>(DuoQGameMode->Starlight->GetController()))
			{
				for (auto Goal : StarlightGoapController->GetGoals())
				{
					Output += TEXT(",") + Goal.Key.name + TEXT(",") + FString::FromInt(Goal.Value) + TEXT(",") + FString::FromInt(Goal.Key.value) + TEXT(";");
				}
				Output += TEXT("|") + StarlightGoapController->GetTagOutputString();
			}
		}
	}
	return Output;
}

FString UCommunicationSubsystem::GenerateDialogueInputString(const FString& PlayerInput)
{
	// add players text
	FString Output;

	if (UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		if (DialogueSubsystem->GetCurrentDollar())
		{
			Output += TEXT("You asked: \"") + DialogueSubsystem->GetCurrentDollar()->Text + TEXT("\". ");
		}
		Output += TEXT("The User responded: \"") + PlayerInput + TEXT("\"");
		Output += TEXT("{\n");
		Output += DialogueSubsystem->GenerateNextDollarsText(DialogueSubsystem->GetCurrentDollarName());
	}

	if(!bFailedLastDialogue && FailurePercentage < FailurePercentThreshold)
	{
		Output += TEXT("\"End\" : \"There is no appropriate response\"\n,");
	}
	// Output += TEXT("\"Bad\" : \"The player said something rude or inappropriate and there is no appropraite response\"\n");

	Output += TEXT("}\nYour response should follow this JSON schema (DO NOT PRETTY PRINT): { \"responseID\" : string } ORDER DOES NOT MATTER - THE LATER CHOICES ARE JUST AS VALID AS THE FIRST ONE, YOU MUST SELECT AN OPTION SENT IN THIS PARTICULAR PROMPT");

	return Output;
}

FString UCommunicationSubsystem::GenerateConvoStartInputString(const FString& PlayerInput)
{
	// add players text
	FString Output;

	if (UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		Output += TEXT("The User asked: \"") + PlayerInput + TEXT("\"");
		Output += TEXT("{\n");
		TArray<FString> Keys;
		DialogueSubsystem->ConvoStarts.GetKeys(Keys);
		for (FString Key : Keys)
		{
			Output += "\"" + Key + "\" : \"" + *DialogueSubsystem->ConvoStarts.Find(Key) + "\",\n";
		}
	}

	Output += TEXT("\"None\" : \"There is no appropriate response\",\n");
	Output += TEXT("\"Bad\" : \"The player said something rude or inappropriate that has no listed appropraite response\"\n");

	Output += TEXT("}\nYour response should follow this JSON schema (DO NOT PRETTY PRINT): { \"responseID\" : string } ORDER DOES NOT MATTER - THE LATER CHOICES ARE JUST AS VALID AS THE FIRST ONE");

	return Output;
}

void UCommunicationSubsystem::OnRecognitionEnd()
{
	UE_LOG(LogCommunication, Display, TEXT("Made it to the Recognition End Delegate"));
}

void UCommunicationSubsystem::OnRecognitionError(const FString& ShortError, const FString& LongError)
{
	// print error message
	UE_LOG(LogCommunication, Error, TEXT("%s"), *LongError);
}

void UCommunicationSubsystem::OnRecognitionProgress(int32 Progress)
{
	RecognitionProgress = Progress;
	if (Progress < 100)
	{
		bRecognitionInProgress = true;
	}
	else
	{
		bRecognitionInProgress = false;
	}
}

void UCommunicationSubsystem::OnRecognizedText(const FString& RecognizedWords)
{
	UE_LOG(LogCommunication, Display, TEXT("Recognized Words: %s"), *RecognizedWords);
	UE_LOG(LogCommunication, Display, TEXT("Recognition Latency: %fms"), (RecognitionComplete - RecognitionStart).GetTotalMilliseconds());

	FString TrimmedWords = RecognizedWords.TrimStartAndEnd();
	if (TrimmedWords.IsEmpty() || TrimmedWords == TEXT("."))
		return;

	OnSpeechRecognitionUpdate.Broadcast(TrimmedWords);
	TrimmedWords += TEXT(" ");
	BufferedWords += TrimmedWords;

	// if this is the last set to be recognized send the words?
	if (!bAlreadySent)
	{
		OnTranscriptionComplete.Broadcast(BufferedWords);
		CleanBufferedWords();
		// Do not send the result if Starlight is talking
		if (UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
		{
			if (UStarlightResponseSubsystem* StarlightResponseSubsystem = GetGameInstance()->GetSubsystem<UStarlightResponseSubsystem>())
			{
				if (StarlightResponseSubsystem->GetStarlightTalking() && !bFailedLastDialogue)
				{
					UE_LOG(LogCommunication, Warning, TEXT("Blocked player speaking when Starlight is talking in a narrative convo"));
					return;
				}
			}
		}
		SendBufferedWords();
		bAlreadySent = true;
	}
}

void UCommunicationSubsystem::SendBufferedWords()
{
	if (BufferedWords.IsEmpty())
	{
		return;
	}

	RecognitionComplete = FDateTime::UtcNow();
	if (UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>())
	{
		FString FormattedString;
		FString ConversationHistory;
		FString SystemPrompt;

		// generate input string based off if in conversation or not
		FormattedString = DialogueSubsystem->GetIsInConversation() ? GenerateDialogueInputString(BufferedWords) : GenerateInputString(BufferedWords);
		FormattedString = FString(TEXT("user: ")).Append(FormattedString);

		// Send the user message to dialogue system for metrics
		DialogueSubsystem->SetLastUserMessage(BufferedWords);

		if (DialogueSubsystem->GetIsInConversation())
		{
			DialogueConversationArray.Add(FormattedString);
			SystemPrompt = DialogueSystemPrompt;
			if (bSendHistoryDialogue)
			{
				ConversationHistory = FString::Join(DialogueConversationArray, TEXT(" "));
			}
			else
			{
				ConversationHistory = FormattedString;
			}
		}
		else
		{
			GOAPConversationArray.Add(FormattedString);
			SystemPrompt = GOAPSystemPrompt;
			if (bSendHistoryCallout)
			{
				ConversationHistory = FString::Join(GOAPConversationArray, TEXT(" "));
			}
			else
			{
				ConversationHistory = FormattedString;
			}
			// also check for open ended convo
			LLMRequest->SendRequest(GenerateConvoStartInputString(BufferedWords), ConvoStartSystemPrompt, this);
		}

		// API Call
		UE_LOG(LogCommunication, Display, TEXT("Formatted Request Words: %s"), *FormattedString);
		LLMRequest->SendRequest(ConversationHistory, SystemPrompt, this);
		OnAPIRequestSent.Broadcast(FormattedString);
	}

	BufferedWords = TEXT("");
}

void UCommunicationSubsystem::CleanBufferedWords()
{
	TArray<FString> SeparatedWords;
	BufferedWords.TrimStartAndEnd().ParseIntoArray(SeparatedWords, TEXT(" "));
	FString RecognizedWordsCleaned = TEXT("");
	FString PrevPunctuation;
	for (FString Word : SeparatedWords)
	{
		if (Word == TEXT(".") || Word == TEXT("!") || Word == TEXT(","))
		{
			PrevPunctuation = Word;
		}
		else
		{
			if (!PrevPunctuation.IsEmpty())
			{
				RecognizedWordsCleaned += PrevPunctuation + TEXT(" ");
				PrevPunctuation.Empty();
			}

			FString ShortenedWord = TEXT("");
			TCHAR	PrevChar = TEXT('\0');
			int		CharCount = 0;
			for (const TCHAR& Character : Word)
			{
				if (Character == PrevChar)
				{
					CharCount++;
				}
				else
				{
					CharCount = 0;
					PrevChar = Character;
				}

				if (CharCount <= 5)
				{
					ShortenedWord += Character;
				}
			}
			RecognizedWordsCleaned += ShortenedWord + TEXT(" ");
		}
	}
	BufferedWords = RecognizedWordsCleaned.TrimStartAndEnd();
}

void UCommunicationSubsystem::OnStartSpeechRecognition(bool bSucceeded)
{
	UE_LOG(LogCommunication, Display, TEXT("Speech Recognition Begin"));
	CapturableSoundWave = UCapturableSoundWave::CreateCapturableSoundWave();
	// CapturableSoundWave->SetInitialDesiredSampleRate(SampleRate);
	LLMRequest = UChatGPTLLMRequest::CreateLLMRequest();
	LLMRequest->AddToRoot();
	LLMRequest->OnRequestComplete.AddDynamic(this, &UCommunicationSubsystem::APIRequestComplete);
	LLMRequest->OnRequestFail.AddDynamic(this, &UCommunicationSubsystem::APIRequestFailed);
	CapturableSoundWave->OnPopulateAudioData.AddDynamic(this, &UCommunicationSubsystem::OnPopulateAudioData);
	CapturableSoundWave->StartCapture(SelectedAudioDeviceID);
	// possible change this later but can't pause bc it flushes the audio and would generate a blank audio
	CapturableSoundWave->ToggleMute(true);
	CapturableSoundWave->GetAvailableAudioInputDevices(DeviceResultDelegate);
}

void UCommunicationSubsystem::OnPopulateAudioData(const TArray<float>& PopulatedAudioData)
{
	if (bUseFlushing)
	{
		if (!bIsFlusing)
		{
			SpeechRecognizer->ProcessAudioData(PopulatedAudioData, CapturableSoundWave->GetSampleRate(), CapturableSoundWave->GetNumberOfChannels(), false);
		}
	}
	else
	{
		SpeechRecognizer->ProcessAudioData(PopulatedAudioData, CapturableSoundWave->GetSampleRate(), CapturableSoundWave->GetNumberOfChannels(), false);
	}
	/*
	SetUpModelTensorBindings(PopulatedAudioData);
	if(ModelInstance->RunSync(InputBindings, OutputBindings) == UE::NNE::EResultStatus::Ok)
	{
		//UE_LOG(LogCommunication,Warning,TEXT("Possibly speaking probablility: %f"),OutputOutputData[0]);

		if(OutputOutputData[0] > RecognitionThreshold)
		{
			// speaking!
			OnSpeechDetected.Broadcast(true);
		}
		else
		{
			// no speaking
			OnSpeechDetected.Broadcast(false);
		}

		InputStateData = TArray(OutputStateNData);
	}
	else
	{
		UE_LOG(LogCommunication,Warning,TEXT("Failed to run VAD model!!!!"));
	}
	*/
}

void UCommunicationSubsystem::APIRequestComplete(UVaRestJsonObject* Request)
{
	ResponseRecieved = FDateTime::UtcNow();
	if (UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>(); DialogueSubsystem && DialogueSubsystem->GetIsInConversation())
	{
		FString DollarName = Request->GetStringField("responseID");
		if (!DollarName.IsEmpty())
		{
			DialogueSubsystem->ActivateDollar(DollarName);
			DialogueConversationArray.Emplace(TEXT("system: ") + Request->EncodeJsonToSingleString());
			bFailedLastDialogue = DollarName == "End";
			FailurePercentage = FailurePercentage * 0.5f + bFailedLastDialogue * 0.5f;
		}
	}
	else
	{
		// grab the array of changes to make to GOAP priorities
		TArray<UVaRestJsonObject*> ResponseObjects = Request->GetObjectArrayField(TEXT("changes"));
		// grab Starlight's goap controller
		if (AStarlightGOAPController* StarlightGoapController = UDuoQBlueprintFunctionLibrary::GetStarlightGOAPController(this))
		{
			for (UVaRestJsonObject* Change : ResponseObjects)
			{
				// name of goal to update
				FString GoalName = Change->GetStringField("goal");
				// new priority of goal to update
				int NewPriority = Change->GetIntegerField("newPriority");
				// Don't break the game starlight
				if (NewPriority <= 0)
					NewPriority = 1;
				// the truth value of the goal.SAMHI I JUST ADDED THIS I DON"T KNOW IF THIS IS CORRECT - Ken This is correct!!
				int truthValue = Change->GetIntegerField("truthValue");

				// creating the updated pair
				std::pair<int, FAtom> Goal;
				Goal.first = NewPriority;
				FAtom Atom;
				Atom.name = GoalName;
				Atom.value = (bool)truthValue;
				Goal.second = Atom;
				
				// force StayPut and ShouldFollowMe to always be true
				if (GoalName.Equals(FString("Stay")))
				{
					Atom.value = true;
					Goal.first = 11;
					NewPriority = 11;
				}
				else if (GoalName.Equals(FString("ShouldFollowMe")))
				{
					Atom.value = true;
				}
				else if (GoalName.Equals(FString("GoToPlayerSpawn")))
				{
					Atom.value = true;
					Goal.first = 20;
					NewPriority = 20;
				}

				// adding updated pairs to the desired world
				StarlightGoapController->desiredWorld.Remove(Atom);
				StarlightGoapController->desiredWorld.FindOrAdd(Atom) = NewPriority;

				StarlightGoapController->bIsPrompted = true;

				// if there is an action that's currently happening, we want to abort that action and replan
				if (UGOAPAction* Action = StarlightGoapController->GetCurrentAction())
				{
					if (UPriorityGOAPAction* PAction = Cast<UPriorityGOAPAction>(Action))
					{
						PAction->SetFinishExecute(true);
					}
				}
				UE_LOG(LogCommunication, Display, TEXT("adding in desiredWorld things"));
			}
			if (!ResponseObjects.IsEmpty())
			{
				if (UStarlightResponseSubsystem* StarlightResponseSubsystem = GetGameInstance()->GetSubsystem<UStarlightResponseSubsystem>())
				{
					//	StarlightResponseSubsystem->HandleCallout(GOTOK,TEXT("Got it!"));
				}
			}
			GOAPConversationArray.Emplace(TEXT("system: ") + Request->EncodeJsonToSingleString());
		}
		if (ResponseObjects.IsEmpty())
		{
			FString BundleName = Request->GetStringField("responseID");
			if (!BundleName.IsEmpty() && BundleName != "None")
			{
				DialogueSubsystem->ActivateBundle(BundleName);
			}
		}
	}

	// log what the response was
	UE_LOG(LogCommunication, Display, TEXT("%s"), *Request->EncodeJsonToSingleString());
	UE_LOG(LogCommunication, Display, TEXT("API Call Latency: %fms"), (ResponseRecieved - RecognitionComplete).GetTotalMilliseconds());
}

void UCommunicationSubsystem::APIRequestFailed(UVaRestRequestJSON* Request)
{
	UE_LOG(LogCommunication, Error, TEXT("API Request Failed"));
	UE_LOG(LogCommunication, Display, TEXT("%s"), *Request->GetResponseContentAsString());
}

void UCommunicationSubsystem::FinishAudioRecording()
{
	RecognitionStart = FDateTime::UtcNow();
	// SpeechRecognizer->ProcessAudioData(FlushBuffer, 16000, 1, false);
	// CapturableSoundWave->AppendAudioDataFromRAW(FlushBuffer,ERuntimeRAWAudioFormat::Int8);
	if (bUseFlushing)
	{
		bIsFlusing = true;
	}
	else
	{
		CapturableSoundWave->ToggleMute(true);
	}

	if (bClearWave)
	{
		CapturableSoundWave = UCapturableSoundWave::CreateCapturableSoundWave();
		CapturableSoundWave->OnPopulateAudioData.AddDynamic(this, &UCommunicationSubsystem::OnPopulateAudioData);
		CapturableSoundWave->StartCapture(SelectedAudioDeviceID);
		CapturableSoundWave->ToggleMute(true);
	}

	bAlreadySent = false;
	SpeechRecognizer->ForceProcessPendingAudioData();

	/*
	// testing rebuild the sound wave but it seems like since it's the same stream it doesn't matter, how do I flush the stream?
	CapturableSoundWave = UCapturableSoundWave::CreateCapturableSoundWave();
	CapturableSoundWave->OnPopulateAudioData.AddDynamic(this, &UCommunicationSubsystem::OnPopulateAudioData);
	CapturableSoundWave->StartCapture(SelectedAudioDeviceID);
	CapturableSoundWave->ToggleMute(true);
	CapturableSoundWave->GetAvailableAudioInputDevices(DeviceResultDelegate);
	*/

	UE_LOG(LogCommunication, Warning, TEXT("Recording Stopped. StillProcessing=%d"), RecognitionProgress);
}

void UCommunicationSubsystem::DebugStringInsert(const FString& Input)
{
	BufferedWords = Input;
	SendBufferedWords();
}
