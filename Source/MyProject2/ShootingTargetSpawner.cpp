#include "ShootingTargetSpawner.h"
#include "AShootingTarget.h"
#include "ShooterGameMode.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

AShootingTargetSpawner::AShootingTargetSpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    DebugMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebugMesh"));
    RootComponent = DebugMesh;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
        TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (MeshAsset.Succeeded())
    {
        DebugMesh->SetStaticMesh(MeshAsset.Object);
    }
    DebugMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
}

void AShootingTargetSpawner::BeginPlay()
{
    Super::BeginPlay();
    SpawnTargets();
}

void AShootingTargetSpawner::SpawnTargets()
{
    if (!TargetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ShootingTargetSpawner: TargetClass is not set"));
        return;
    }

    struct FTargetData
    {
        ETargetOperator Op;
        int32 Value;
    };

    TArray<ETargetOperator> Operators = {
        ETargetOperator::Add,
        ETargetOperator::Subtract,
        ETargetOperator::Multiply,
        ETargetOperator::Divide
    };

    TArray<FTargetData> TargetData;
    float RunningScore = 0.f;

    // always start with Add so there is a base score
    TargetData.Add({ ETargetOperator::Add, FMath::RandRange(10, 50) });
    RunningScore += TargetData[0].Value;

    int32 Attempts = 0;
    while (FMath::Abs(RunningScore) < MinimumGoalScore && TargetData.Num() < MaxTargets && Attempts < 100)
    {
        Attempts++;

        ETargetOperator Op = Operators[FMath::RandRange(0, Operators.Num() - 1)];
        int32 Value = 0;

        if (Op == ETargetOperator::Divide)
        {
            TArray<int32> SafeDivisors = { 2, 3, 4, 5 };
            Value = SafeDivisors[FMath::RandRange(0, SafeDivisors.Num() - 1)];
        }
        else if (Op == ETargetOperator::Multiply)
        {
            Value = FMath::RandRange(2, 5);
        }
        else
        {
            Value = FMath::RandRange(10, 50);
        }

        // simulate what this would do to the score
        float TestScore = RunningScore;
        switch (Op)
        {
            case ETargetOperator::Add:      TestScore += Value; break;
            case ETargetOperator::Subtract: TestScore -= Value; break;
            case ETargetOperator::Multiply: TestScore *= Value; break;
            case ETargetOperator::Divide:
                if (Value != 0) TestScore /= Value;
                break;
        }

        // only add this target if it keeps the score positive and moving toward goal
        if (TestScore > 0)
        {
            TargetData.Add({ Op, Value });
            RunningScore = TestScore;
        }
    }

    // if we still haven't hit minimum, pad with Add targets
    while (FMath::Abs(RunningScore) < MinimumGoalScore && TargetData.Num() < MaxTargets)
    {
        int32 Value = FMath::RandRange(10, 50);
        TargetData.Add({ ETargetOperator::Add, Value });
        RunningScore += Value;
    }

    int32 GoalScore = MinimumGoalScore;

    AShooterGameMode* GM = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode());
    if (GM)
    {
        GM->SetGoalScore(GoalScore);
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow,
            FString::Printf(TEXT("Goal Score: %d | Max Possible: %d | Targets: %d"),
                GoalScore, FMath::RoundToInt(RunningScore), TargetData.Num()));
    }

    for (const FTargetData& Data : TargetData)
    {
        float RandomX = FMath::FRandRange(-SpawnAreaHalfSize, SpawnAreaHalfSize);
        float RandomY = FMath::FRandRange(-SpawnAreaHalfSize, SpawnAreaHalfSize);

        FVector SpawnLocation = GetActorLocation() + FVector(RandomX, RandomY, 0.f);
        FRotator SpawnRotation = GetActorRotation();

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        AShootingTarget* Target = GetWorld()->SpawnActor<AShootingTarget>(
            TargetClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (Target)
        {
            Target->SetOperatorAndValue(Data.Op, Data.Value);
        }
    }
}