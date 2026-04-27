#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "AShootingTarget.generated.h"

UENUM()
enum class ETargetOperator : uint8
{
	Add,
	Subtract,
	Multiply,
	Divide
};

UCLASS()
class MYPROJECT2_API AShootingTarget : public AActor
{
	GENERATED_BODY()

public:
	AShootingTarget();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* TargetMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UTextRenderComponent* LabelText;

	UPROPERTY()
	int32 TargetValue;

	UPROPERTY()
	ETargetOperator Operator;

	UFUNCTION(BlueprintCallable, Category = "Target")
	void SetLabelText(const FString& NewText);

	UFUNCTION()
	void OnHit();

	// called by spawner after all targets are placed
	void SetOperatorAndValue(ETargetOperator InOperator, int32 InValue);
};