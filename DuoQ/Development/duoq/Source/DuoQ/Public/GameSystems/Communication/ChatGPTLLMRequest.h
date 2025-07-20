// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystems/Communication/LLMRequest.h"
#include "ChatGPTLLMRequest.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API UChatGPTLLMRequest : public ULLMRequest
{
	GENERATED_BODY()
public:
	virtual void SendRequest(FString ConversationHistory, FString Prompt, class UCommunicationSubsystem* CommunicationSubsystem) override;

	static ULLMRequest* CreateLLMRequest();
protected:
	virtual void CompleteRequest(class UVaRestRequestJSON* Request) override;

private:
	uint8 LLMModelIndex = 0;

	FString CurrConversationHistory;
	FString CurrPrompt;
	class UCommunicationSubsystem* CurrCommunicationSubsystem;
};
