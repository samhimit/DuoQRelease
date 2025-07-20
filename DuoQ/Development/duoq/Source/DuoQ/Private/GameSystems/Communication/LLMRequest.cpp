// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/Communication/LLMRequest.h"

#include "DuoQBlueprintFunctionLibrary.h"
#include "VaRestSubsystem.h"
#include "GameSystems/DuoQGameInstance.h"
#include "GameSystems/Communication/CommunicationSubsystem.h"

void ULLMRequest::SendRequest(FString ConversationHistory, FString Prompt, UCommunicationSubsystem* CommunicationSubsystem)
{
	if(UVaRestSubsystem* VaRestSubsystem = GEngine->GetEngineSubsystem<UVaRestSubsystem>())
	{
		VaRestRequest = VaRestSubsystem->ConstructVaRestRequestExt(EVaRestRequestVerb::POST,EVaRestRequestContentType::json);
		VaRestRequest->OnRequestComplete.AddDynamic(this,&ULLMRequest::CompleteRequest);
		VaRestRequest->OnRequestFail.AddDynamic(this,&ULLMRequest::FailRequest);
	}
}

void ULLMRequest::CompleteRequest(UVaRestRequestJSON* Request)
{
	UE_LOG(LogCommunication, Display, TEXT("Response Recieved"));
	if(Request->GetResponseContentAsString().Contains(TEXT("exceeded")))
	{
		UE_LOG(LogCommunication, Error, TEXT("%s"),*Request->GetResponseContentAsString());
	}
}

void ULLMRequest::FailRequest(UVaRestRequestJSON* Request)
{
	OnRequestFail.Broadcast(Request);
}

ULLMRequest* ULLMRequest::CreateLLMRequest()
{
	if (!IsInGameThread())
	{
		return nullptr;
	}

	return NewObject<ULLMRequest>();
}