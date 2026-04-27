#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AShootingTarget.h"
#include "ShootingTargetSpawner.generated.h"

UCLASS()
class MYPROJECT2_API AShootingTargetSpawner : public AActor
{
    GENERATED_BODY()

public:
    AShootingTargetSpawner();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Debug")
    UStaticMeshComponent* DebugMesh;

    UPROPERTY(EditAnywhere, Category = "Spawner|Targets")
    TSubclassOf<AShootingTarget> TargetClass;

    UPROPERTY(EditAnywhere, Category = "Spawner|Targets", meta = (ClampMin = 1, ClampMax = 100))
    int32 MinTargets = 3;

    UPROPERTY(EditAnywhere, Category = "Spawner|Targets", meta = (ClampMin = 1, ClampMax = 100))
    int32 MaxTargets = 10;

    // if true, goal score is randomly generated above RandomGoalScoreMin
    UPROPERTY(EditAnywhere, Category = "Spawner|Score")
    bool bRandomGoalScore = true;

    // minimum value for the random goal score
    UPROPERTY(EditAnywhere, Category = "Spawner|Score", meta = (ClampMin = 10, EditCondition = "bRandomGoalScore"))
    int32 RandomGoalScoreMin = 10;

    // maximum value for the random goal score
    UPROPERTY(EditAnywhere, Category = "Spawner|Score", meta = (ClampMin = 10, EditCondition = "bRandomGoalScore"))
    int32 RandomGoalScoreMax = 500;

    // fixed goal score used when bRandomGoalScore is false
    UPROPERTY(EditAnywhere, Category = "Spawner|Score", meta = (ClampMin = 10, EditCondition = "!bRandomGoalScore"))
    int32 FixedGoalScore = 100;

    // spawn area half size on X axis in cm
    UPROPERTY(EditAnywhere, Category = "Spawner|Area", meta = (ClampMin = 100, ClampMax = 50000, Units = "cm"))
    float SpawnAreaHalfSizeX = 500.f;

    // spawn area half size on Y axis in cm
    UPROPERTY(EditAnywhere, Category = "Spawner|Area", meta = (ClampMin = 100, ClampMax = 50000, Units = "cm"))
    float SpawnAreaHalfSizeY = 500.f;

    // height offset for spawned targets
    UPROPERTY(EditAnywhere, Category = "Spawner|Area", meta = (Units = "cm"))
    float SpawnHeightOffset = 0.f;

    // random height variation added to each target
    UPROPERTY(EditAnywhere, Category = "Spawner|Area", meta = (ClampMin = 0, Units = "cm"))
    float SpawnHeightVariance = 0.f;

    // value range for Add targets
    UPROPERTY(EditAnywhere, Category = "Spawner|Values", meta = (ClampMin = 1))
    int32 AddValueMin = 10;

    UPROPERTY(EditAnywhere, Category = "Spawner|Values", meta = (ClampMin = 1))
    int32 AddValueMax = 50;

    // value range for Subtract targets
    UPROPERTY(EditAnywhere, Category = "Spawner|Values", meta = (ClampMin = 1))
    int32 SubtractValueMin = 10;

    UPROPERTY(EditAnywhere, Category = "Spawner|Values", meta = (ClampMin = 1))
    int32 SubtractValueMax = 50;

    // value range for Multiply targets
    UPROPERTY(EditAnywhere, Category = "Spawner|Values", meta = (ClampMin = 2))
    int32 MultiplyValueMin = 2;

    UPROPERTY(EditAnywhere, Category = "Spawner|Values", meta = (ClampMin = 2))
    int32 MultiplyValueMax = 5;

    // whether to allow subtract and divide operators
    UPROPERTY(EditAnywhere, Category = "Spawner|Operators")
    bool bAllowSubtract = true;

    UPROPERTY(EditAnywhere, Category = "Spawner|Operators")
    bool bAllowMultiply = true;

    

    void SpawnTargets();
};