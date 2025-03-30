// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cell.h"
#include "GameGrid.generated.h"





UCLASS()
class PAA_GAME_API AGameGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameGrid();

	//Grid size
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	int32 GridSize = 25;
	//Cell size
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	float CellSize;
	//Obstacle percentage (set by default at 15%)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings", meta = (ClampMin = "0", ClampMax = "99"))
	float ObstaclePercentage;

	//2D matrix of cells
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TArray<ACell*> GridMatrix;

	// Class of cell to spawn
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid")
	TSubclassOf<ACell> CellClass;

	//Grid assets loaded at grid construction and passed to the objects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	UStaticMesh* DefaultCellMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	UMaterialInterface* DefaultMaterialNormal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	TArray<UMaterialInterface*> DefaultObstacleMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	UMaterialInterface* DefaultHighlight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	UMaterialInterface* DefaultMovementHighlight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	UMaterialInterface* DefaultAttackHighlight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	UStaticMesh* DefaultUnitMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	UMaterialInterface* HumanBrawler;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	UMaterialInterface* HumanSniper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	UMaterialInterface* AiBrawler;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	UMaterialInterface* AiSniper;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason);
	
	// Initialize empty grid
	UFUNCTION(BlueprintCallable)
	void InitializeGrid();

	// Generate obstacles in specified percentage
	UFUNCTION(BlueprintCallable)
	void GenerateObstacles();

	// Test for correct grid
	UFUNCTION(BlueprintCallable)
	bool IsGridFullyAccessible() const;





};
