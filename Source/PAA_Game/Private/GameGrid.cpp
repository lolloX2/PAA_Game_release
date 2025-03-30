// Fill out your copyright notice in the Description page of Project Settings.


#include "GameGrid.h"



// Sets default values
AGameGrid::AGameGrid()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    GridSize;
    CellSize = 100.f;
    ObstaclePercentage = 15.f;
    CellClass = ACell::StaticClass();


    //Loading cell mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CellMeshAsset(TEXT("StaticMesh'/Game/Shapes/Cell/Shape_Plane.Shape_Plane'"));
    if (CellMeshAsset.Succeeded())
    {
        DefaultCellMesh = CellMeshAsset.Object;
        DefaultUnitMesh = CellMeshAsset.Object;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load default cell mesh"));
    }

    // Loading normal cell material
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> DefaultMaterialNormalAsset(TEXT("Material'/Game/Materials/MTile.MTile'"));
    if (DefaultMaterialNormalAsset.Succeeded())
    {
        DefaultMaterialNormal = DefaultMaterialNormalAsset.Object;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load Default Tile Material"));
    }
    //Loading Obstacle materials
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> DefaultMaterialTree1Asset(TEXT("Material'/Game/Materials/MTree1.MTree1'"));
    if (DefaultMaterialTree1Asset.Succeeded())
    {
        DefaultObstacleMaterials.Add(DefaultMaterialTree1Asset.Object);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load Obstacle Material"));
    }
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> DefaultMaterialTree2Asset(TEXT("Material'/Game/Materials/MTree2.MTree2'"));
    if (DefaultMaterialTree2Asset.Succeeded())
    {
        DefaultObstacleMaterials.Add(DefaultMaterialTree2Asset.Object);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load Obstacle Material"));
    }
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> DefaultMaterialMountainAsset(TEXT("Material'/Game/Materials/MMountain.MMountain'"));
    if (DefaultMaterialMountainAsset.Succeeded())
    {
        DefaultObstacleMaterials.Add(DefaultMaterialMountainAsset.Object);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load Obstacle Material"));
    }
    //Loading highlight material
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> DefaultMaterialHighlightAsset(TEXT("Material'/Game/Materials/MTileH.MTileH'"));
    if (DefaultMaterialHighlightAsset.Succeeded())
    {
        DefaultHighlight = DefaultMaterialHighlightAsset.Object;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load Default Tile highlight Material"));
    }
    //Loading movement highlight material
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> DefaultMaterialMovementHighlightAsset(TEXT("Material'/Game/Materials/MTileM.MTileM'"));
    if (DefaultMaterialMovementHighlightAsset.Succeeded())
    {
        DefaultMovementHighlight = DefaultMaterialMovementHighlightAsset.Object;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load Default Tile movement Material"));
    }
    //Loading attackable units highlight material
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> DefaultMaterialAttackHighlightAsset(TEXT("Material'/Game/Materials/MTileA.MTileA'"));
    if (DefaultMaterialAttackHighlightAsset.Succeeded())
    {
        DefaultAttackHighlight = DefaultMaterialAttackHighlightAsset.Object;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load Default Tile attack Material"));
    }
    //Loading Human Brawler material
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> HumanBrawlerAsset(TEXT("Material'/Game/Materials/MBrawlerH.MBrawlerH'"));
    if (HumanBrawlerAsset.Succeeded())
    {
        HumanBrawler = HumanBrawlerAsset.Object;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load Human Brawler Material"));
    }
    //Loading Human Sniper material
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> HumanSniperAsset(TEXT("Material'/Game/Materials/MSniperH.MSniperH'"));
    if (HumanSniperAsset.Succeeded())
    {
        HumanSniper = HumanSniperAsset.Object;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load Human sniper Material"));
    }
    //Loading AI Brawler material
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> AiBrawlerAsset(TEXT("Material'/Game/Materials/MBrawlerAI.MBrawlerAI'"));
    if (AiBrawlerAsset.Succeeded())
    {
        AiBrawler = AiBrawlerAsset.Object;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load ai Brawler Material"));
    }
    //Loading AI Sniper material
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> AiSniperAsset(TEXT("Material'/Game/Materials/MSniperAI.MSniperAI'"));
    if (AiSniperAsset.Succeeded())
    {
        AiSniper = AiSniperAsset.Object;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load ai sniper Material"));
    }
}
// Called when the game starts or when spawned
void AGameGrid::BeginPlay()
{
	Super::BeginPlay();
	InitializeGrid();
	GenerateObstacles();
}
void AGameGrid::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Clears cells at game end
    for (ACell* Cell : GridMatrix)
    {
        if (Cell)
        {
            Cell->Destroy();
        }
    }
    GridMatrix.Empty();

    Super::EndPlay(EndPlayReason);
}

void AGameGrid::InitializeGrid()
{

    // Clears the grid destroying existing cells
    for (ACell* Cell : GridMatrix)
    {
        if (Cell)
        {
            Cell->Destroy();
        }
    }
    GridMatrix.Empty();
    //Calculate the measure of each side of the grid
    float GridSide = GridSize * CellSize;

    // Get the actor position to compute starting location (upper left corner)
    FVector Origin = GetActorLocation();
    FVector StartLocation = Origin - FVector(GridSide * 0.5f, GridSide * 0.5f, 0.f);

    for (int32 RowIndex = 0; RowIndex < GridSize; ++RowIndex)
    {
        for (int32 ColIndex = 0; ColIndex < GridSize; ++ColIndex)
        {
           
            // Computing cell location to center it
            FVector CellLocation = StartLocation + FVector(ColIndex * CellSize + CellSize * 0.5f, RowIndex * CellSize + CellSize * 0.5f, 10.f);
            // Actor spawning parameters
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            // Spawning cell
            ACell* NewCell = GetWorld()->SpawnActor<ACell>(CellClass, CellLocation, FRotator::ZeroRotator, SpawnParams);
            if (NewCell)
            {
                // Setting cell coordinates in the grid
                NewCell->Row = RowIndex;
                NewCell->Column = ColIndex;

                NewCell->MaterialNormal = DefaultMaterialNormal;
                NewCell->MaterialObstacle = DefaultObstacleMaterials;
                NewCell->Highlight = DefaultHighlight;

                if (NewCell->CellMesh && DefaultCellMesh)
                {
                    NewCell->CellMesh->SetStaticMesh(DefaultCellMesh);
                }
                NewCell->SetCellType(0);
                GridMatrix.Add(NewCell);
            }
        }
    }
}


void AGameGrid::GenerateObstacles()
    {
        // Computes total cell number and obstacle cells based on set percentage
        int32 TotalCells = GridMatrix.Num();
        int32 TargetObstacles = FMath::RoundToInt(TotalCells * (ObstaclePercentage / 100.f));
        int32 CurrentObstacles = 0;

        // Sets a maximum number of obstacle placement attempts to avoid endless looping
        int32 MaxAttempts = TotalCells * 10;
        int32 Attempts = 0;

        // Sets all cells free at beginning
        for (ACell* Cell : GridMatrix)
        {
            if (Cell)
            {
                Cell->SetCellType(false);
            }
        }
        
        // Loop for adding obstacles until target number is reached or attempts are finished
        while (CurrentObstacles < TargetObstacles && Attempts < MaxAttempts)
        {
            // Select a random non-obstacle cell
            int32 Index = FMath::RandRange(0, TotalCells - 1);
            ACell* CandidateCell = GridMatrix[Index];

            if (CandidateCell && !CandidateCell->bIsObstacle)
            {
                // Set candidate cell to obstacle
                CandidateCell->SetCellType(true);

                // Checks if the map is connected with DFS
                if (IsGridFullyAccessible())
                {
                    // If map is connected obstacle is valid, increase counter
                    CurrentObstacles++;
                }
                else
                {
                    // Otherwise reset cell and counter isn't increased
                    CandidateCell->SetCellType(false);
                }
            }
            //Increasing attempts counter
            Attempts++;
        }

        // Notifying whether target obstacle number couldn't be reached
        if (CurrentObstacles < TargetObstacles)
        {
            UE_LOG(LogTemp, Warning, TEXT("Non è stato possibile generare il numero desiderato di ostacoli mantenendo la mappa connessa. Ostacoli attuali: %d / %d"), CurrentObstacles, TargetObstacles);
        }
    }
bool AGameGrid::IsGridFullyAccessible() const
{
    // Checking if grid is spawned correctly
    if (GridMatrix.Num() == 0)
    {
        return false;
    }

    TArray<bool> Visited;
    Visited.Init(false, GridSize * GridSize); //Initialize array of cell number elements

    // Starts from a free cell
    int32 StartIndex = INDEX_NONE;
    for (int32 i = 0; i < GridMatrix.Num(); ++i)
    {
        if (GridMatrix[i] && !GridMatrix[i]->bIsObstacle)
        {
            StartIndex = i;
            break;
        }
    }

    if (StartIndex == INDEX_NONE)
    {
        return false;
    }

    // Initializing stack for DFS
    TArray<int32> Stack;
    Stack.Push(StartIndex);
    Visited[StartIndex] = true;

    // Directions for nearby cells
    const int32 DRow[4] = { -1, 1, 0, 0 };
    const int32 DCol[4] = { 0, 0, -1, 1 };

    // Iterative DFS
    while (Stack.Num() > 0)
    {
        int32 CurrentIndex = Stack.Pop();
        // Retrieves current cell
        ACell* CurrentCell = GridMatrix[CurrentIndex];
        if (!CurrentCell)
        {
            continue;
        }

        int32 CurrentRow = CurrentCell->Row;
        int32 CurrentCol = CurrentCell->Column;

        // Explores neighbors
        for (int32 Dir = 0; Dir < 4; Dir++)
        {
            int32 NewRow = CurrentRow + DRow[Dir];
            int32 NewCol = CurrentCol + DCol[Dir];

            // Checking if we're within grid bounds
            if (NewRow >= 0 && NewRow < GridSize && NewCol >= 0 && NewCol < GridSize)
            {
                int32 NeighborIndex = NewRow * GridSize + NewCol;
                // Sets neighbor as visited if it isn't already and is not an obstacle
                if (!Visited[NeighborIndex] && GridMatrix[NeighborIndex] && !GridMatrix[NeighborIndex]->bIsObstacle)
                {
                    Visited[NeighborIndex] = true;
                    Stack.Push(NeighborIndex);
                }
            }
        }
    }

    // If all non obstacle cells are visited map is connected
    for (int32 i = 0; i < GridMatrix.Num(); ++i)
    {
        if (GridMatrix[i] && !GridMatrix[i]->bIsObstacle && !Visited[i])
        {
            //If we find a free cell unvisited map is not connected
            return false;
        }
    }

    return true;
}

/* Called every frame
void AGameGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}*/

