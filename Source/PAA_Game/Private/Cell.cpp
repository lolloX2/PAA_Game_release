// Fill out your copyright notice in the Description page of Project Settings.


#include "Cell.h"
#include "PAA_GameMode.h"

// Sets default values
ACell::ACell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    // Creating and setting the mesh as root component
    CellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CellMesh"));
    RootComponent = CellMesh;

    // Default values for the cell
    Row = 0;
    Column = 0;
    bIsObstacle = false;
    bOccupied = false;
    MaterialNormal = nullptr;
    Highlight = nullptr;

    CellMesh->OnBeginCursorOver.AddDynamic(this, &ACell::OnCellBeginCursorOver);
    CellMesh->OnEndCursorOver.AddDynamic(this, &ACell::OnCellEndCursorOver);
    CellMesh->OnClicked.AddDynamic(this, &ACell::OnCellClicked);
}

// Called when the game starts or when spawned
void ACell::BeginPlay()
{
	Super::BeginPlay();
	
}


void ACell::SetCellType(bool bNewType)
{
    {
        bIsObstacle = bNewType;

        if (CellMesh)
        {
            if (bIsObstacle)
            {
                // If the cell is an obstacle select a random material
                if (MaterialObstacle.Num() > 0)
                {
                    int32 RandomIndex = FMath::RandRange(0, MaterialObstacle.Num() - 1);
                    UMaterialInterface* SelectedMaterial = MaterialObstacle[RandomIndex];

                    if (SelectedMaterial)
                    {
                        CellMesh->SetMaterial(0, SelectedMaterial);
                    }
                }
            }
            else if (MaterialNormal)
            {
                //Setting normal cell material
                CellMesh->SetMaterial(0, MaterialNormal);
            }
            CellMesh->SetRelativeScale3D(FVector(0.95f, 0.95f, 1.0f));
        }
    }
}

void ACell::OnCellClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
    UE_LOG(LogTemp, Warning, TEXT("CLIC SU CELLA"));
    // If the cell is not an obstacle sets the clicked cell value in the game mode to manage placement and movement logic
    APAA_GameMode* GM = Cast<APAA_GameMode>(GetWorld()->GetAuthGameMode());
    if (GM) {
        if (GM->ClickedUnit != nullptr) {
            if(!GM->ReachableCells.Contains(this) && !GM->AttackableUnits.Contains(this)) {
                //Clears highlights if clicking on an empty cell
                GM->WipeMovement();
                GM->WipeTargets();
            }
            else if (GM->ReachableCells.Contains(this))
            {
                // Moves clicked unit to selected cell
                GM->ClickedUnit->MoveToCell(this);
                GM->WipeMovement();
                GM->WipeTargets();
            }
        }
        else {
            if (!bIsObstacle)
            {
                // Sets Clicked cell if there is none when clicking on a non-obstacle cell
                GM->ClickedCell = this;               
            }
        }
    }
}

void ACell::OnCellBeginCursorOver(UPrimitiveComponent* TouchedComponent)
{
    // Sets highlight material if cell is free
    APAA_GameMode* GM = Cast<APAA_GameMode>(GetWorld()->GetAuthGameMode());
    if (GM)
    {
        if (GM->ClickedUnit == nullptr) {

            if (!bIsObstacle && Highlight)
            {
                CellMesh->SetMaterial(0, Highlight);
            }
        }
    }
}

void ACell::OnCellEndCursorOver(UPrimitiveComponent* TouchedComponent)
{
    //Resets normal material when cursor moves away
    APAA_GameMode* GM = Cast<APAA_GameMode>(GetWorld()->GetAuthGameMode());
    if (GM)
    {
        if (GM->ClickedUnit == nullptr) {

            if (!bIsObstacle && MaterialNormal)
            {
                CellMesh->SetMaterial(0, MaterialNormal);
            }
        }
    }
}

FString ACell::GetCellID() const
{
    TCHAR ColChar = TEXT('A') + Column;
    int32 RowNum = Row + 1;
    return FString::Printf(TEXT("%c%d"), ColChar, RowNum);
}