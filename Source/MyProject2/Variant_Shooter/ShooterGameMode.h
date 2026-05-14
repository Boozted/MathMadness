// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AShootingTarget.h"
#include "ShooterGameInstance.h"
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

	UPROPERTY()
	bool bGoalReached = false;

	UPROPERTY()
	int32 LevelNumber = 1;

	UPROPERTY(EditAnywhere, Category="Shooter")
	FName NextLevelName = FName("Lvl_Shooter");
	
	UPROPERTY(EditAnywhere, Category="Shooter")
	float StartingTime = 120.f; 

	UPROPERTY()
	float TimeRemaining = 0.f;

	FTimerHandle CountdownTimer;

	void TickCountdown();
	void OnTimerExpired();

protected:

	virtual void BeginPlay() override;

public:

	void IncrementTeamScore(uint8 TeamByte);

	void AddScore(int32 Amount);

	void ApplyTargetOperator(ETargetOperator Op, int32 Value);

	void SetGoalScore(int32 InGoal);

	

	int32 GetScore() const { return FMath::RoundToInt(CurrentScore); }

	int32 GetGoalScore() const { return GoalScore; }

	void CheckGoal();

	void LogScoreToFile();
};