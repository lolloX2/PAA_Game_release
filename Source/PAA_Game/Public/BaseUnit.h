// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cell.h"
#include "BaseUnit.generated.h"

// Enum for attack type
UENUM(BlueprintType)
enum class EAttackType : uint8
{
	Melee   UMETA(DisplayName = "Melee"),
	Ranged  UMETA(DisplayName = "Ranged")
};


UCLASS()
class PAA_GAME_API ABaseUnit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseUnit();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    //Mesh for visualization
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cell")
    UStaticMeshComponent* UnitMesh;

    //----------Movement animation related variables
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    bool bIsMoving;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    FVector MovementTarget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
    float MovementSpeed;

    TArray<FVector> Waypoints;

    bool bHasMoved;

    bool bHasAttacked;



    //----------Unit Parameters--------

   
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit Stats")
    int32 Movement;

  
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit Stats")
    EAttackType AttackType;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit Stats")
    int32 AttackRange;

    //Damage range
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit Stats")
    int32 DamageMin;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit Stats")
    int32 DamageMax;

    // Starting unit HP
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit Stats")
    int32 HitPoints;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit Stats")
    ACell* OccupiedCell;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit Stats")
    bool bTeam;

    //-----------Unit functions----------------
    // Returns a random damage value in range
    UFUNCTION(BlueprintCallable, Category = "Unit Actions")
    int32 CalculateDamage() const;

    // Applies damage to unit and eventually triggers counterattack mechanic
    UFUNCTION(BlueprintCallable, Category = "Unit Actions")
    void ReceiveDamage(int32 DamageAmount);
    // Returns random damage in the counterattack interval (1-3)
    UFUNCTION(BlueprintCallable, Category = "Unit Actions")
    int32 CalculateCounterAttack();
    // Performs the attack logic, called upon clicking on a targetable unit
    UFUNCTION(BlueprintCallable, Category = "Unit Actions")
    void AttackUnit(ABaseUnit* TargetUnit);

    UFUNCTION(BlueprintCallable, Category = "Unit Actions")
    bool IsAlive() const;
    // Manages unit clicking behaviour
    UFUNCTION()
    void OnUnitClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
    // Manages moving logic, called upon clicking on a reachable cell
    UFUNCTION()
    void MoveToCell(ACell* DestinationCell);
    // Sets path for realistic movement animation
    UFUNCTION()
    void SetMovementPath(const TArray<ACell*>& Path);
    // Functions for writing moves in the on-screen move history
    void LogUnitMove(bool bPlayer, ABaseUnit* Unit, ACell* From, ACell* To);
    void LogUnitAttack(bool bPlayer, ABaseUnit* Unit, ACell* TargetCell, int32 Damage);

};
