#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AmicusWeakPoint.generated.h"

// Declare the delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeakPointDestroyed, FLinearColor, WeakPointColor);

UCLASS()
class DUOQ_API AAmicusWeakPoint : public AActor
{
	GENERATED_BODY()

public:
	AAmicusWeakPoint();

	// Public color variable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weak Point")
	FLinearColor WeakPointColor;

	// Health variable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weak Point")
	float Health = 100.0f;

	// Delegate to notify destruction
	UPROPERTY(BlueprintAssignable, Category = "Weak Point")
	FOnWeakPointDestroyed OnWeakPointDestroyed;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* HitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);
};
