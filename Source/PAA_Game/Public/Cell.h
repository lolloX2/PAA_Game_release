// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cell.generated.h"

class ABaseUnit;

UCLASS()
class PAA_GAME_API ACell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	/* Called every frame
	virtual void Tick(float DeltaTime) override;
	*/
	// Boolean value to define whether the cell is an obstacle
	bool bIsObstacle;
	//Boolean value to define the cell occupation
	bool bOccupied;
	// Pointer to occupying unit
	ABaseUnit* OccupyingUnit;

	//Mesh for visualization
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cell")
	UStaticMeshComponent* CellMesh;

	//Position indexes
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cell")
	int32 Row;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cell")
	int32 Column;

	// Materials for normal cells, obstacle and mouse hover highlighting
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cell")
	UMaterialInterface* MaterialNormal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cell")
	UMaterialInterface* Highlight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cell")
	TArray<UMaterialInterface*> MaterialObstacle;

	// Sets cell type and updates material
	void SetCellType(bool newType);

	UFUNCTION()
	void OnCellClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

	// Mouse events functions
	UFUNCTION()
	void OnCellBeginCursorOver(UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
	void OnCellEndCursorOver(UPrimitiveComponent* TouchedComponent);
	// Returns Cell name in Letter-Number format
	UFUNCTION(BlueprintCallable, Category = "Cell")
	FString GetCellID() const;

};
