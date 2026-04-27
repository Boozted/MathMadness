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

    int32 TargetCountLimit = FMath::RandRange(MinTargets, MaxTargets);

    TArray<FTargetData> TargetData;
    float RunningScore = 0.f;
    int32 TrueMaxPossible = 0;

    // always start with Add
    int32 FirstValue = FMath::RandRange(AddValueMin, AddValueMax);
    TargetData.Add({ ETargetOperator::Add, FirstValue });
    RunningScore += FirstValue;
    TrueMaxPossible += FirstValue;

    // phase 1 — build score with Add and Multiply
    int32 BuildPhaseCount = FMath::Max(1, TargetCountLimit / 2);
    for (int32 i = 1; i < BuildPhaseCount; i++)
    {
        bool bUseMultiply = bAllowMultiply && FMath::RandRange(0, 9) < 3;
        ETargetOperator Op = bUseMultiply ? ETargetOperator::Multiply : ETargetOperator::Add;
        int32 Value = bUseMultiply
            ? FMath::RandRange(MultiplyValueMin, MultiplyValueMax)
            : FMath::RandRange(AddValueMin, AddValueMax);

        float TestScore = RunningScore;
        if (Op == ETargetOperator::Multiply) TestScore += Value * 10;
        else TestScore += Value;

        if (TestScore <= 100000.f)
        {
            TargetData.Add({ Op, Value });
            RunningScore = TestScore;
            if (Op == ETargetOperator::Add) TrueMaxPossible += Value;
            else if (Op == ETargetOperator::Multiply) TrueMaxPossible += Value * 10;
        }
        else
        {
            int32 AddVal = FMath::RandRange(AddValueMin, AddValueMax);
            TargetData.Add({ ETargetOperator::Add, AddVal });
            RunningScore += AddVal;
            TrueMaxPossible += AddVal;
        }
    }

    // phase 2 — fill remaining with Add and Subtract only
    while (TargetData.Num() < TargetCountLimit)
    {
        TArray<ETargetOperator> SafeOps;
        SafeOps.Add(ETargetOperator::Add);

        if (bAllowSubtract && RunningScore - SubtractValueMax > 10)
            SafeOps.Add(ETargetOperator::Subtract);

        ETargetOperator Op = SafeOps[FMath::RandRange(0, SafeOps.Num() - 1)];
        int32 Value = Op == ETargetOperator::Subtract
            ? FMath::RandRange(SubtractValueMin, SubtractValueMax)
            : FMath::RandRange(AddValueMin, AddValueMax);

        float TestScore = RunningScore;
        if (Op == ETargetOperator::Subtract) TestScore -= Value;
        else TestScore += Value;

        if (TestScore > 0)
        {
            TargetData.Add({ Op, Value });
            RunningScore = TestScore;
            if (Op == ETargetOperator::Add) TrueMaxPossible += Value;
        }
    }

    int32 MaxPossible = TrueMaxPossible;
    int32 GoalScore = 0;

    if (bRandomGoalScore)
    {
        float Percentage = FMath::FRandRange(0.4f, 0.8f);
        GoalScore = FMath::Max(10, FMath::RoundToInt(MaxPossible * Percentage));

        int32 ClampedMin = FMath::Max(10, RandomGoalScoreMin);
        int32 ClampedMax = FMath::Max(ClampedMin, RandomGoalScoreMax);
        GoalScore = FMath::Clamp(GoalScore, ClampedMin, ClampedMax);

        if (GoalScore > MaxPossible)
            GoalScore = FMath::Max(10, FMath::RoundToInt(MaxPossible * 0.6f));
    }
    else
    {
        GoalScore = FMath::Max(10, FixedGoalScore);

        if (GoalScore > MaxPossible)
        {
            UE_LOG(LogTemp, Warning, TEXT("ShootingTargetSpawner: Fixed goal %d is higher than max possible %d — clamping"), GoalScore, MaxPossible);
            GoalScore = FMath::Max(10, FMath::RoundToInt(MaxPossible * 0.6f));
        }
    }

    AShooterGameMode* GM = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode());
    if (GM)
    {
        GM->SetGoalScore(GoalScore);
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow,
            FString::Printf(TEXT("Goal Score: %d | Max Possible: %d | Targets: %d"),
                GoalScore, MaxPossible, TargetData.Num()));
    }

    for (const FTargetData& Data : TargetData)
    {
        float RandomX = FMath::FRandRange(-SpawnAreaHalfSizeX, SpawnAreaHalfSizeX);
        float RandomY = FMath::FRandRange(-SpawnAreaHalfSizeY, SpawnAreaHalfSizeY);
        float RandomZ = SpawnHeightOffset + FMath::FRandRange(0.f, SpawnHeightVariance);

        FVector SpawnLocation = GetActorLocation() + FVector(RandomX, RandomY, RandomZ);
        FRotator SpawnRotation = GetActorRotation();

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        AShootingTarget* Target = GetWorld()->SpawnActor<AShootingTarget>(
            TargetClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (Target)
            Target->SetOperatorAndValue(Data.Op, Data.Value);
    }
}