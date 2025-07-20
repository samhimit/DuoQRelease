// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Characters/Player/DuoQPlayerCharacter.h"
#include "Characters/Partner/StarlightCharacter.h"
#include "Components/BoxComponent.h" 
#include "Elevator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBothPlayersInElevator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FElevatorMoved);

UCLASS()
class DUOQ_API AElevator : public AActor
{
	GENERATED_BODY()

public:
	// Constructor
	AElevator();

protected:
	// BeginPlay override
	virtual void BeginPlay() override;

public:
	// Tick override
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* RootComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* TriggerBox;

	
	UPROPERTY(BlueprintReadOnly)
	FVector Start;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MakeEditWidget=true))
	FVector End;
	
	// Movement variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elevator")
	float MoveDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elevator")
	float MoveSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Elevator")
	bool bShouldMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elevator")
	class UBoxComponent* BoxCollider;

	bool bIsAtStart;
	
	bool bPlayerInBox;
	bool bStarlightInBox;

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void MoveElevator();

	UPROPERTY(BlueprintAssignable)
	FBothPlayersInElevator OnBothPlayersInElevator;

	UPROPERTY(BlueprintAssignable)
	FElevatorMoved OnElevatorMoved;
	
};
