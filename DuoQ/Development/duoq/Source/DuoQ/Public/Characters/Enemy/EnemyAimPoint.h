// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "EnemyAimPoint.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUOQ_API UEnemyAimPoint : public USceneComponent
{
	GENERATED_BODY()

public:	
	UEnemyAimPoint();
};
