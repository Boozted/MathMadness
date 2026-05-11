#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ShooterGameInstance.generated.h"

UCLASS()
class MYPROJECT2_API UShooterGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 LevelNumber = 1;
	
	UPROPERTY()
	int32 LastGoalScore = 0;

	UPROPERTY()
	float GoalScoreMultiplier = 1.0f;

	void AdvanceLevel()
	{
		LevelNumber++;
		float Increase = FMath::FRandRange(0.10f, 0.15f);
		GoalScoreMultiplier *= (1.0f + Increase);
	}
};