// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/Communication/ChatGPTLLMRequest.h"

#include "DuoQBlueprintFunctionLibrary.h"
#include "VaRestJsonObject.h"
#include "GameSystems/Communication/CommunicationSubsystem.h"
#include "VaRestSubsystem.h"

void UChatGPTLLMRequest::SendRequest(FString ConversationHistory, FString Prompt, UCommunicationSubsystem* CommunicationSubsystem)
{
	Super::SendRequest(ConversationHistory, Prompt, CommunicationSubsystem);

	CurrConversationHistory = ConversationHistory;
	CurrPrompt = Prompt;
	CurrCommunicationSubsystem = CommunicationSubsystem;
	
	if(UVaRestSubsystem* VaRestSubsystem = GEngine->GetEngineSubsystem<UVaRestSubsystem>())
	{
		TArray<UVaRestJsonObject*> Messages;
	
		Messages.Emplace(VaRestSubsystem->ConstructVaRestJsonObject());
		Messages.Emplace(VaRestSubsystem->ConstructVaRestJsonObject());
		// add system prompt
		Messages[0]->SetStringField(TEXT("role"),TEXT("system"));
		Messages[0]->SetStringField(TEXT("content"),Prompt);

		// add user prompt and history
		Messages[1]->SetStringField(TEXT("role"),TEXT("user"));
		Messages[1]->SetStringField(TEXT("content"),ConversationHistory);

		UVaRestJsonObject* RequestJSON = VaRestSubsystem->ConstructVaRestJsonObject();
		
		UVaRestJsonObject* ResponseFormat = VaRestSubsystem->ConstructVaRestJsonObject();
		ResponseFormat->SetStringField(TEXT("type"),TEXT("json_object"));
		RequestJSON->SetObjectField(TEXT("response_format"),ResponseFormat);
		
		RequestJSON->SetObjectArrayField(TEXT("messages"),Messages);
		RequestJSON->SetNumberFieldDouble(TEXT("max_completion_tokens"),CommunicationSubsystem->MaxTokens);
		RequestJSON->SetNumberFieldDouble(TEXT("temperature"),CommunicationSubsystem->Temperature);
		RequestJSON->SetNumberFieldDouble(TEXT("top_p"),CommunicationSubsystem->Top_p);
		#if !UE_BUILD_SHIPPING
		RequestJSON->SetBoolField(TEXT("is_dev"),true);
		#endif
		
		VaRestRequest->SetRequestObject(RequestJSON);
		VaRestRequest->SetHeader(TEXT("Content-Type"),TEXT("application/json"));
	
		VaRestRequest->ProcessURL(CommunicationSubsystem->ServerEndpoint);
	}
}

ULLMRequest* UChatGPTLLMRequest::CreateLLMRequest()
{
	if (!IsInGameThread())
	{
		return nullptr;
	}

	return NewObject<UChatGPTLLMRequest>();
}

void UChatGPTLLMRequest::CompleteRequest(UVaRestRequestJSON* Request)
{
	UE_LOG(LogCommunication, Display, TEXT("Response Recieved"));
	if ((Request->GetResponseCode() >= 500 || Request->GetResponseCode() == 401 || Request->GetResponseCode() == 429) && CurrCommunicationSubsystem)
	{
		FailRequest(Request);
	}
	else if(UVaRestSubsystem* VaRestSubsystem = GEngine->GetEngineSubsystem<UVaRestSubsystem>())
	{
		UVaRestJsonObject* Response = Request->GetResponseObject();
		TArray<UVaRestJsonObject*> ChoicesArray = Response->GetObjectArrayField(TEXT("choices"));
		if(ChoicesArray.Num() > 0)
		{
			// get response and finish reason
			UVaRestJsonObject* MessageObject = ChoicesArray[0]->GetObjectField(TEXT("message"));
			FString ResponseString = MessageObject->GetStringField(TEXT("content"));
			FString FinishReason = ChoicesArray[0]->GetStringField(TEXT("finish_reason"));

			// log
			//UE_LOG(LogCommunication, Display, TEXT("API response: %s"),*ResponseString);
			UE_LOG(LogCommunication, Display, TEXT("Finish reason: %s"),*FinishReason);

			// decode response json (this is what the LLM is generating reference the prompt for the format)
			UVaRestJsonObject* ContentObject = VaRestSubsystem->ConstructVaRestJsonObject();
			//getting response string from LLM and create a Json
			ContentObject->DecodeJson(ResponseString);
			OnRequestComplete.Broadcast(ContentObject);
		}
	}
	else
	{
		FailRequest(Request);
	}
}