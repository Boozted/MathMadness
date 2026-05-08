#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterPortal.generated.h"

UCLASS()
class MYPROJECT2_API AShooterPortal : public AActor
{
	GENERATED_BODY()

public:
	AShooterPortal();
	
	void ShowPortal();
    void HidePortal();
    
    UPROPERTY(EditAnywhere, Category = "Portal")
    bool bStartHidden = true;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* PortalMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, Category = "Portal")
	FName NextLevelName = FName("Lvl_FirstPerson");

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};