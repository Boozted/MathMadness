// Copyright Epic Games, Inc. All Rights Reserved.

#include "Variant_Shooter/ShooterGameMode.h"
#include "ShooterUI.h"
#include "AShootingTarget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

void AShooterGameMode::BeginPlay()
{
    Super::BeginPlay();

    ShooterUI = CreateWidget<UShooterUI>(UGameplayStatics::GetPlayerController(GetWorld(), 0), ShooterUIClass);
    ShooterUI->AddToViewport(0);
}

void AShooterGameMode::IncrementTeamScore(uint8 TeamByte)
{
    int32 Score = 0;
    if (int32* FoundScore = TeamScores.Find(TeamByte))
    {
        Score = *FoundScore;
    }

    ++Score;
    TeamScores.Add(TeamByte, Score);

    ShooterUI->BP_UpdateScore(TeamByte, Score);
}

void AShooterGameMode::AddScore(int32 Amount)
{
    CurrentScore += Amount;
    UE_LOG(LogTemp, Warning, TEXT("Score: %d / Goal: %d"), GetScore(), GoalScore);
    CheckGoal();
}

void AShooterGameMode::ApplyTargetOperator(ETargetOperator Op, int32 Value)
{
    switch (Op)
    {
        case ETargetOperator::Add:      CurrentScore += Value; break;
        case ETargetOperator::Subtract: CurrentScore -= Value; break;
        case ETargetOperator::Multiply: CurrentScore *= Value; break;
        case ETargetOperator::Divide:   
            if (Value != 0) CurrentScore /= Value; 
            break;
    }

    UE_LOG(LogTemp, Warning, TEXT("Score: %d / Goal: %d"), GetScore(), GoalScore);
    CheckGoal();
}

void AShooterGameMode::CheckGoal()
{
    TArray<AActor*> RemainingTargets;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShootingTarget::StaticClass(), RemainingTargets);

    if (RemainingTargets.Num() == 0)
    {
        if (GetScore() >= GoalScore)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
                FString::Printf(TEXT("Goal reached! Head to the portal. Score: %d / %d"), GetScore(), GoalScore));
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
                FString::Printf(TEXT("Not enough score. Got %d, needed %d"), GetScore(), GoalScore));
        }
    }
}