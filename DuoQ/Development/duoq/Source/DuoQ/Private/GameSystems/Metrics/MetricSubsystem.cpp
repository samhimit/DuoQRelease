// Copyright 2024


#include "GameSystems/Metrics/MetricSubsystem.h"

#include "GameSystems/Communication/CommunicationSubsystem.h"

void UMetricSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	FDateTime CurrentTime = FDateTime::Now();

	int32 Year = CurrentTime.GetYear();
	int32 Month = CurrentTime.GetMonth();
	int32 Day = CurrentTime.GetDay();
	int32 Hour = CurrentTime.GetHour();
	int32 Minute = CurrentTime.GetMinute();

	Time = FString::Printf(TEXT("%04d-%02d-%02d %02d-%02d"), Year, Month, Day, Hour, Minute);

	IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString Directory = FPaths::ProjectSavedDir() + TEXT("/Metrics");
	if (!platformFile.DirectoryExists(*Directory))
	{
		platformFile.CreateDirectory(*Directory);
	}

	// Creating the Dialogue Conversation File
	DialoguePathName = Directory + TEXT("/DialogueConversation-") + Time + TEXT(".txt");
	FFileHelper::SaveStringToFile(*Time, *DialoguePathName, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append | FILEWRITE_EvenIfReadOnly);

	// Creating the GOAP Conversation File
	GOAPPathName = Directory + TEXT("/GOAPConversation-") + Time + TEXT(".txt");
	FFileHelper::SaveStringToFile(*Time, *GOAPPathName, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append | FILEWRITE_EvenIfReadOnly);

	// Creating the Failed Dollar Log File
	FailedDollarPathName = Directory + TEXT("/FailedDollar-") + Time + TEXT(".txt");
	FFileHelper::SaveStringToFile(*Time, *FailedDollarPathName, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append | FILEWRITE_EvenIfReadOnly);
}


void UMetricSubsystem::Deinitialize()
{
	Super::Deinitialize();
	
	bool bWrittenToFile = true;
	if(UCommunicationSubsystem* CommunicationSubsystem = GetGameInstance()->GetSubsystem<UCommunicationSubsystem>())
	{
		// save out full dialogue history
		bWrittenToFile &=FFileHelper::SaveStringArrayToFile(CommunicationSubsystem->DialogueConversationArray,*DialoguePathName,FFileHelper::EEncodingOptions::AutoDetect,&IFileManager::Get(),FILEWRITE_Append | FILEWRITE_EvenIfReadOnly);
		// save out entire goap history
		bWrittenToFile &= FFileHelper::SaveStringArrayToFile(CommunicationSubsystem->GOAPConversationArray,*GOAPPathName,FFileHelper::EEncodingOptions::AutoDetect,&IFileManager::Get(),FILEWRITE_Append | FILEWRITE_EvenIfReadOnly);

		// Save FailedDollars to metrics file
		FString FailedDollarData;

		// Iterate through the FailedDollars map and append each entry to the string
		for (const auto& Pair : FailedDollars)
		{
			FString Dollar = Pair.Key;
			int32 Count = Pair.Value;
			FString DataToLog = FString::Printf(TEXT("Failed Dollar: %s, Count: %d\n"), *Dollar, Count);
			FailedDollarData += DataToLog;
		}

		if (!FailedDollarData.IsEmpty())
		{
			// Append the failed dollar data to the log file
			bWrittenToFile &= FFileHelper::SaveStringToFile(FailedDollarData, *FailedDollarPathName, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append | FILEWRITE_EvenIfReadOnly);
		}
	}
	if(!bWrittenToFile)
		UE_LOG(LogTemp,Error,TEXT("Could not write to file"));
}

// Write to file the failed dollar and user message
void UMetricSubsystem::TrackFailedDollar(FString Dollar, FString UserMessage)
{
	// Update the count of failed dollars
	FailedDollars.FindOrAdd(Dollar) += 1;

	// Prepare the data to be saved into the file
	FString Timestamp = FDateTime::Now().ToString();
	FString DataToLog = FString::Printf(TEXT("[%s] Failed Dollar: %s, User Message: %s\n"), *Timestamp, *Dollar, *UserMessage);

	// Ensure the directory for logging exists
	IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString Directory = FPaths::ProjectSavedDir() + TEXT("/Metrics");
	if (!platformFile.DirectoryExists(*Directory))
	{
		platformFile.CreateDirectory(*Directory);
	}
	
	// Append the log data to the file
	bool bWrittenToFile = FFileHelper::SaveStringToFile(DataToLog, *FailedDollarPathName, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append | FILEWRITE_EvenIfReadOnly);

	if (!bWrittenToFile)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to write failed dollar data to metrics file"));
	}
}
