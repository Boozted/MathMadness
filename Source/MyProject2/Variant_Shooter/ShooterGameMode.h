#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AShootingTarget.h"
#include "ShooterGameMode.generated.h"

class UShooterUI;

UCLASS(abstract)
class MYPROJECT2_API AShooterGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category="Shooter")
	TSubclassOf<UShooterUI> ShooterUIClass;

	TObjectPtr<UShooterUI> ShooterUI;

	TMap<uint8, int32> TeamScores;

	UPROPERTY()
	float CurrentScore = 0.f;

	UPROPERTY()
	int32 GoalScore = 0;

	// level to load when goal is reached
	UPROPERTY(EditAnywhere, Category="Shooter")
	FName NextLevelName = FName("NextLevel");

protected:

	virtual void BeginPlay() override;

public:

	void IncrementTeamScore(uint8 TeamByte);

	void AddScore(int32 Amount);

	void ApplyTargetOperator(ETargetOperator Op, int32 Value);

	void SetGoalScore(int32 InGoal) { GoalScore = InGoal; }

	int32 GetScore() const { return FMath::RoundToInt(CurrentScore); }

	int32 GetGoalScore() const { return GoalScore; }

	void CheckGoal();
};