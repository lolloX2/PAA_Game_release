// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include <GameGrid.h>
#include "Camera/CameraActor.h"
#include "Brawler.h"
#include "Sniper.h"
#include "UserInterface.h"
#include "PAA_GameMode.generated.h"

UENUM()
enum class EGamePhase {
	Setup   UMETA(DisplayName = "Setup"),
	Combat  UMETA(DisplayName = "Combat")
};


UCLASS()
class PAA_GAME_API APAA_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APAA_GameMode();
	virtual void BeginPlay() override;
	// Reference to grid
	UPROPERTY(VisibleAnywhere, Category = "Game")
	AGameGrid* GameGrid;

	// Grid class
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	TSubclassOf<AGameGrid> GameGridClass;

	// Unit classes
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	TSubclassOf<ABrawler> BrawlerClass;
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	TSubclassOf<ASniper> SniperClass;
	//Placed units counter for setup phase
	int32 PlacedUnits;
	//Arrays containing units (one per team)
	TArray<ABaseUnit*> HpUnits;
	TArray<ABaseUnit*> AiUnits;

	
	//Unit placement function
	UFUNCTION()
	void PlaceUnit(ACell* SelectedCell, bool bTeam, bool bClass);
	//Value to set the first unit to spawn
	UPROPERTY(BlueprintReadOnly)
	bool bPlaceSniperFirst;
	//Called by the switch unit button
	UFUNCTION(BlueprintCallable)
	void SwitchFirstUnit(bool SniperFirst);
	UFUNCTION()
	void CoinFlip();
	//Game initialization
	UFUNCTION()
	void GameSetup();
	//Function for passing turn
	UFUNCTION(BlueprintCallable)
	void NextTurn();

	UFUNCTION(BlueprintCallable)
	bool GetTurn();

	UFUNCTION(BlueprintCallable)
	bool GetPhase();
	//Checks if setup phase is done
	bool SetupEnd() const;
	//Movement functions
	UFUNCTION()
	void ShowMovementRange(ABaseUnit* SelectedUnit);

	TArray<ACell*> CalculateMovement(ACell* StartCell, int32 MovementRange);

	TArray<ACell*> FindPath(ACell* StartCell, ACell* EndCell);

	//Gets cell neighbors, used in BFS algorithms
	TArray<ACell*> GetNeighbors(ACell* Cell);
	//Array for saving reachable cells 
	TArray<ACell*> ReachableCells;
	//Array for saving attackable units
	TArray<ACell*> AttackableUnits;
	//Highlight clearing functions
	UFUNCTION(BlueprintCallable)
	void WipeMovement();
	UFUNCTION(BlueprintCallable)
	void WipeTargets();
	//Attack functions
	UFUNCTION()
	void ShowAttackRange(ABaseUnit* SelectedUnit);

	UFUNCTION()
	TArray<ACell*> GetAttackTargets(ACell* StartCell, int32 AttackRange);

	UFUNCTION()
	ABaseUnit* ChooseClosestEnemy(ACell* StartCell);

	UFUNCTION()
	ACell* MoveTowardsEnemy(ABaseUnit* MovingUnit);
	//resets action counter
	UFUNCTION()
	void ResetUnitActions();
	//checks action counter
	UFUNCTION()
	bool UnitActionsFinished(bool Player);
	//Game over functions
	UFUNCTION()
	void GameOver(bool Winner);

	UFUNCTION()
	void DrawOutcome();

	//Boolean value for starting player: TRUE = Human, FALSE = AI
	UPROPERTY(VisibleAnywhere, Category = "Game")
	bool bStartingPlayer;

	UPROPERTY(VisibleAnywhere, Category = "Game")
	bool bCurrentPlayer;

	UPROPERTY(VisibleAnywhere, Category = "Game")
	EGamePhase CurrentPhase;
	//Pointer to clicked cell
	UPROPERTY(VisibleAnywhere, Category = "Game")
	ACell* ClickedCell;
	//pointer to clicked unit
	UPROPERTY(VisibleAnywhere, Category = "Game")
	ABaseUnit* ClickedUnit;

	void Tick(float DeltaTime);
	//AI turn function
	void ExecuteAiTurn();

	//Remaining actions counter
	int32 UnitActions;
	//Reference to user interface
	UPROPERTY()
	UUserInterface* UI;
	//Class for User interface
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserInterface> UIClass;

protected:
	// Sets top down view on grid
	void SetupTopDownCamera();
private:
	// Reference to camera actor
	ACameraActor* TopDownCamera;
	
};
