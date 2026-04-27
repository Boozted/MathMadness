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

	UPROPERTY(EditAnywhere, Category = "Spawner")
	TSubclassOf<AShootingTarget> TargetClass;

	UPROPERTY(EditAnywhere, Category = "Spawner", meta = (ClampMin = 3, ClampMax = 10))
	int32 MinTargets = 3;

	UPROPERTY(EditAnywhere, Category = "Spawner", meta = (ClampMin = 3, ClampMax = 10))
	int32 MaxTargets = 10;

	UPROPERTY(EditAnywhere, Category = "Spawner", meta = (ClampMin = 100, ClampMax = 10000))
	float SpawnAreaHalfSize = 500.f;
	
	UPROPERTY(EditAnywhere, Category = "Spawner", meta = (ClampMin = 0))
	int32 MinimumGoalScore = 100;

	void SpawnTargets();
};