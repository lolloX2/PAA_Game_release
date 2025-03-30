// Fill out your copyright notice in the Description page of Project Settings.

#include "PAA_GameMode.h"
#include "Camera/CameraActor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include <Kismet/GameplayStatics.h>


APAA_GameMode::APAA_GameMode()
{
    PrimaryActorTick.bCanEverTick = true;
    GameGrid = nullptr;
    TopDownCamera = nullptr;
    DefaultPawnClass = nullptr;
    bStartingPlayer = false;
    bCurrentPlayer = false;
    bPlaceSniperFirst = false;
    BrawlerClass = ABrawler::StaticClass();
    SniperClass = ASniper::StaticClass();
    PlacedUnits = 0;
    ClickedCell = nullptr;
    ClickedUnit = nullptr;
    CurrentPhase = EGamePhase::Setup;
    UnitActions = 4;
    static ConstructorHelpers::FClassFinder<UUserInterface> UIBPClass(TEXT("/Game/UserInterface/BP_UserInterface"));
    if (UIBPClass.Class != nullptr)
    {
        UIClass = UIBPClass.Class;
    }
}

void APAA_GameMode::BeginPlay()
{
    Super::BeginPlay();

    // Cerca tutti gli attori di classe AGameGrid nel mondo
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameGrid::StaticClass(), FoundActors);

    if (FoundActors.Num() > 0)
    {
        // Assegna la prima istanza trovata alla variabile GameGrid
        GameGrid = Cast<AGameGrid>(FoundActors[0]);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Nessun attore AGameGrid trovato nel mondo."));
    }
    if (!UI)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
        if (PC)
        {
            // Aggiungi il widget HUD al viewport
            UI = CreateWidget<UUserInterface>(PC, UIClass);
            if (UI)
            {

                UI->AddToViewport();
                
            }
        }
    }

    SetupTopDownCamera();
    GameSetup();

}

void APAA_GameMode::PlaceUnit(ACell* SelectedCell, bool bTeam, bool bClass)
{
    if (!SelectedCell || SelectedCell->bOccupied || SelectedCell->bIsObstacle)
    {
        return;
    }

    if (BrawlerClass && SniperClass)
    {
        FActorSpawnParameters SpawnParams;
        // La posizione dello spawn può essere quella della cella, eventualmente aggiustata per centrare l'unità
        FVector SpawnLocation = SelectedCell->GetActorLocation() + FVector(0.f, 0.f, 5.f);
        FRotator SpawnRotation = FRotator::ZeroRotator;
        if (bCurrentPlayer) {
            if (bPlaceSniperFirst) {
                bClass = !bClass;
            }
        }
        if (bClass) {
            ABrawler* NewUnit = GetWorld()->SpawnActor<ABrawler>(BrawlerClass, SpawnLocation, SpawnRotation, SpawnParams);
            if (NewUnit)
            {
                // Una volta spawnata l'unità, marca la cella come occupata
                SelectedCell->bOccupied = true;
                NewUnit->OccupiedCell = SelectedCell;
                SelectedCell->OccupyingUnit = NewUnit;
                NewUnit->bTeam = bCurrentPlayer;
                NewUnit->UnitMesh->SetStaticMesh(GameGrid->DefaultUnitMesh);
                PlacedUnits++;
                UI->DisplayHealth(NewUnit, 40);
                if (bCurrentPlayer)
                {
                    HpUnits.Add(NewUnit);
                }
                else
                {
                    AiUnits.Add(NewUnit);
                }
                if (bTeam)
                {
                    NewUnit->UnitMesh->SetMaterial(0, GameGrid->HumanBrawler);
                }
                else
                {
                    NewUnit->UnitMesh->SetMaterial(0, GameGrid->AiBrawler);
                }
                // Puoi anche salvare il riferimento all'unità in un array se necessario per la logica di gioco
                // Esempio: Player1Units.Add(NewUnit) oppure Player2Units.Add(NewUnit) in base al turno
            }
        }
        else {
            ASniper* NewUnit = GetWorld()->SpawnActor<ASniper>(SniperClass, SpawnLocation, SpawnRotation, SpawnParams);
            if (NewUnit)
            {
                // Una volta spawnata l'unità, marca la cella come occupata
                SelectedCell->bOccupied = true;
                NewUnit->OccupiedCell = SelectedCell;
                SelectedCell->OccupyingUnit = NewUnit;
                NewUnit->bTeam = bCurrentPlayer;
                NewUnit->UnitMesh->SetStaticMesh(GameGrid->DefaultUnitMesh);
                UI->DisplayHealth(NewUnit, 20);
                PlacedUnits++;
                if (bCurrentPlayer)
                {
                    HpUnits.Add(NewUnit);
                }
                else
                {
                    AiUnits.Add(NewUnit);
                }
                if (bTeam)
                {
                    NewUnit->UnitMesh->SetMaterial(0, GameGrid->HumanSniper);
                }
                else
                {
                    NewUnit->UnitMesh->SetMaterial(0, GameGrid->AiSniper);
                }
                // Puoi anche salvare il riferimento all'unità in un array se necessario per la logica di gioco
                // Esempio: Player1Units.Add(NewUnit) oppure Player2Units.Add(NewUnit) in base al turno
            }
        }

    }
    else UE_LOG(LogTemp, Warning, TEXT("IMPOSSIBILE SPAWNARE UNITA, NON ESISTE LA CLASSE."));
}
void APAA_GameMode::SwitchFirstUnit(bool SniperFirst)
{
    bPlaceSniperFirst = SniperFirst;
    UI->SetFirstUnitText(SniperFirst); //Updating switch unit button
}
void APAA_GameMode::CoinFlip()
{//Getting random boolean value
    bStartingPlayer = FMath::RandBool();
    if (bStartingPlayer) {
        UI->AddLogEntry("Inizia il Giocatore");
    }
    else
        UI->AddLogEntry("Inizia l'IA");

}
void APAA_GameMode::GameSetup()
{
    CoinFlip(); //Selects first player
    CurrentPhase = EGamePhase::Setup;
    bCurrentPlayer = bStartingPlayer; //Setting first player turn
    if (bStartingPlayer) {
        UI->SetTurnText(FText::FromString("Turno: Umano"));
    } else UI->SetTurnText(FText::FromString("Turno: AI"));
}
void APAA_GameMode::NextTurn()
{
    //Changing current player and updating text
    bCurrentPlayer = !bCurrentPlayer;
    if (bCurrentPlayer) {
        UI->SetTurnText(FText::FromString("Turno: Umano"));
    }
    else {
        UI->SetTurnText(FText::FromString("Turno: AI"));
    }
    //Clears clicked cell
    ClickedCell = nullptr;
    //Resetting actions counter
    ResetUnitActions();
    UE_LOG(LogTemp, Warning, TEXT("Turno passato"));
}
bool APAA_GameMode::GetTurn()
{
    //returns current player to enable pass button
    return bCurrentPlayer;
}
bool APAA_GameMode::GetPhase()
{
    //returns true if in combat to enable pass button
    if (CurrentPhase == EGamePhase::Combat) {
        return true;
    }else 
    return false;
}
bool APAA_GameMode::SetupEnd() const
{
    //checks for end of setup (when all of the 4 units are placed)
    return (
        (PlacedUnits == 4) && (HpUnits.Num() == AiUnits.Num())
        );
}
void APAA_GameMode::ShowMovementRange(ABaseUnit* SelectedUnit)
{
    //Gets starting cell and movement from the selected unit
    ACell* StartingCell = SelectedUnit->OccupiedCell;
    int32 MovementRange = SelectedUnit->Movement;
    //Calculated reachable cells using BFS
    ReachableCells = CalculateMovement(StartingCell, MovementRange);
    for (ACell* Cell : ReachableCells) {
        //Highlghts reachable cells
        Cell->CellMesh->SetMaterial(0, GameGrid->DefaultMovementHighlight);
    }
}

TArray<ACell*> APAA_GameMode::CalculateMovement(ACell* StartCell, int32 MovementRange)
{
    TArray<ACell*> ValidCells;
    if (!StartCell)
    {
        return ValidCells;
    }

    // BFS Queue
    TQueue<ACell*> Queue;
    // Map for tracking cell distance
    TMap<ACell*, int32> DistanceMap;

    Queue.Enqueue(StartCell);
    DistanceMap.Add(StartCell, 0);

    do
    {
        ACell* CurrentCell;
        Queue.Dequeue(CurrentCell);

        int32 CurrentDistance = DistanceMap[CurrentCell];
        if (CurrentDistance <= MovementRange)
        {
            ValidCells.Add(CurrentCell);
        }

        // If we are at maximum range stops exploring
        if (CurrentDistance == MovementRange)
        {
            continue;
        }

        // Getting neighbor cells
        TArray<ACell*> Neighbors = GetNeighbors(CurrentCell);
        for (ACell* Neighbor : Neighbors)
        {
            // If neighbor exists, is free and non-obstacle add it to map
            if (Neighbor && !DistanceMap.Contains(Neighbor) && !Neighbor->bIsObstacle && !Neighbor->bOccupied)
            {
                DistanceMap.Add(Neighbor, CurrentDistance + 1);
                Queue.Enqueue(Neighbor);
            }
        }
    } while (!Queue.IsEmpty());
    return ValidCells;
}
TArray<ACell*> APAA_GameMode::FindPath(ACell* StartCell, ACell* EndCell)
{
    TArray<ACell*> Path;  // Path to be returned
    if (!StartCell || !EndCell)
    {
        UE_LOG(LogTemp, Warning, TEXT("FindPath: StartCell o EndCell non validi."));
        return Path;
    }

    // Queue map and set for BFS
    TQueue<ACell*> Queue;
    TMap<ACell*, ACell*> CameFrom;
    TSet<ACell*> Visited;

    // Starting from first cell
    Queue.Enqueue(StartCell);
    Visited.Add(StartCell);

    bool bFound = false;
    while (!Queue.IsEmpty())
    {
        ACell* Current = nullptr;
        Queue.Dequeue(Current);
        if (Current == EndCell)
        {
            bFound = true;
            break;
        }

        // getting neighbors
        TArray<ACell*> Neighbors = GetNeighbors(Current);
        for (ACell* Neighbor : Neighbors)
        {
            // Skip cell if is visited or occupied or obstacle
            if (Neighbor && !Visited.Contains(Neighbor) && !Neighbor->bOccupied && !Neighbor->bIsObstacle)
            {
                Queue.Enqueue(Neighbor);
                Visited.Add(Neighbor);
                CameFrom.Add(Neighbor, Current);
            }
        }
    }

    if (bFound)
    {
        // reconstructing path
        ACell* Current = EndCell;
        while (Current)
        {
            Path.Insert(Current, 0); // Insert at front
            if (CameFrom.Contains(Current))
            {
                Current = CameFrom[Current];
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("FindPath: Nessun percorso trovato."));
    }

    return Path;
}

TArray<ACell*> APAA_GameMode::GetNeighbors(ACell* Cell)
{
    TArray<ACell*> Neighbors;
    if (!Cell)
    {
        return Neighbors;
    }

    int32 r = Cell->Row;
    int32 c = Cell->Column;

    // cell above
    if (r > 0)
    {
        int32 NeighborIndex = (r - 1) * GameGrid->GridSize + c;
        if (GameGrid->GridMatrix.IsValidIndex(NeighborIndex))
        {
            Neighbors.Add(GameGrid->GridMatrix[NeighborIndex]);
        }
    }

    // cell below
    if (r < GameGrid->GridSize - 1)
    {
        int32 NeighborIndex = (r + 1) * GameGrid->GridSize + c;
        if (GameGrid->GridMatrix.IsValidIndex(NeighborIndex))
        {
            Neighbors.Add(GameGrid->GridMatrix[NeighborIndex]);
        }
    }

    // cell to the left
    if (c > 0)
    {
        int32 NeighborIndex = r * GameGrid->GridSize + (c - 1);
        if (GameGrid->GridMatrix.IsValidIndex(NeighborIndex))
        {
            Neighbors.Add(GameGrid->GridMatrix[NeighborIndex]);
        }
    }

    // cell to the right
    if (c < GameGrid->GridSize - 1)
    {
        int32 NeighborIndex = r * GameGrid->GridSize + (c + 1);
        if (GameGrid->GridMatrix.IsValidIndex(NeighborIndex))
        {
            Neighbors.Add(GameGrid->GridMatrix[NeighborIndex]);
        }
    }

    return Neighbors;
}
void APAA_GameMode::WipeMovement()
{
    //clears clicked unit pointer and highlights
    ClickedUnit = nullptr;
    for (ACell* Cell : ReachableCells) {
        Cell->CellMesh->SetMaterial(0, GameGrid->DefaultMaterialNormal);
    }
    //clears reachable cells array
    ReachableCells.Empty();
}

void APAA_GameMode::WipeTargets()
{
    //clears clicked unit pointer and highlights
    ClickedUnit = nullptr;
    for (ACell* Cell : AttackableUnits) {
        Cell->CellMesh->SetMaterial(0, GameGrid->DefaultMaterialNormal);
    }
    //clears targets array
    AttackableUnits.Empty();
}

void APAA_GameMode::ShowAttackRange(ABaseUnit* SelectedUnit)
{
    UE_LOG(LogTemp, Warning, TEXT("showing attack range"));
    ACell* StartCell = SelectedUnit->OccupiedCell;
    int32 Range = SelectedUnit->AttackRange;
    AttackableUnits = GetAttackTargets(StartCell, Range);
    if (AttackableUnits.Num() == 0) {
        UE_LOG(LogTemp, Warning, TEXT("No units found"));
    }
    for (ACell* Cell : AttackableUnits) {
        UE_LOG(LogTemp, Warning, TEXT("highlighting enemy target unit"));
        Cell->CellMesh->SetMaterial(0, GameGrid->DefaultAttackHighlight);
    }
}

TArray<ACell*> APAA_GameMode::GetAttackTargets(ACell* StartCell, int32 AttackRange)
{
    UE_LOG(LogTemp, Warning, TEXT("looking for enemy units in range"));
    TArray<ACell*> AttackTargets;
    if (!StartCell)
    {
        return AttackTargets;
    }

    // queue and distance map for BFS
    TQueue<ACell*> Queue;
    TMap<ACell*, int32> DistanceMap;

    Queue.Enqueue(StartCell);
    DistanceMap.Add(StartCell, 0);

    while (!Queue.IsEmpty())
    {
        ACell* CurrentCell = nullptr;
        Queue.Dequeue(CurrentCell);

        int32 CurrentDistance = DistanceMap[CurrentCell];

        // if cell is occupied by an enemy unit set as a target
        if (CurrentCell->bOccupied && CurrentCell->OccupyingUnit != nullptr)
        {
            if (bCurrentPlayer != CurrentCell->OccupyingUnit->bTeam) {
                UE_LOG(LogTemp, Warning, TEXT("found enemy unit"));
                AttackTargets.Add(CurrentCell);

            }
        }

        // Sexpand to neighbors if still in range
        if (CurrentDistance < AttackRange)
        {
            TArray<ACell*> Neighbors = GetNeighbors(CurrentCell);
            for (ACell* Neighbor : Neighbors)
            {
                if (Neighbor && !DistanceMap.Contains(Neighbor))
                {
                    DistanceMap.Add(Neighbor, CurrentDistance + 1);
                    Queue.Enqueue(Neighbor);
                }
            }
        }
    }
    return AttackTargets;
}

ABaseUnit* APAA_GameMode::ChooseClosestEnemy(ACell* StartCell)
{
    ABaseUnit* BestEnemy = nullptr;
    int32 BestDistance = MAX_int32;

    // Cycle on enemy (human) units
    for (ABaseUnit* Enemy : HpUnits)
    {
        if (Enemy && Enemy->OccupiedCell)
        {
            //Selects closest unit
            int32 Distance = FMath::Abs(StartCell->Row - Enemy->OccupiedCell->Row) +
                FMath::Abs(StartCell->Column - Enemy->OccupiedCell->Column);
            if (Distance < BestDistance)
            {
                BestDistance = Distance;
                BestEnemy = Enemy;
            }
        }
    }
    return BestEnemy;
}

ACell* APAA_GameMode::MoveTowardsEnemy(ABaseUnit* MovingUnit)
{
    if (!MovingUnit || !MovingUnit->OccupiedCell || !GameGrid)
    {
        return nullptr;
    }

    ACell* StartCell = MovingUnit->OccupiedCell;

    if (ReachableCells.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Nessuna cella raggiungibile."));
        return nullptr;
    }

    // Select closest unit
    ABaseUnit* TargetEnemy = ChooseClosestEnemy(StartCell);
    if (!TargetEnemy || !TargetEnemy->OccupiedCell)
    {
        UE_LOG(LogTemp, Warning, TEXT("Nessuna unità nemica valida trovata."));
        return nullptr;
    }
    ACell* TargetEnemyCell = TargetEnemy->OccupiedCell;

    // Moves to closest cell to target unit
    ACell* BestCell = nullptr;
    int32 BestDistance = MAX_int32;

    for (ACell* Cell : ReachableCells)
    {
        if (!Cell)
            continue;
        //Calculating manhattan distance
        int32 Distance = FMath::Abs(Cell->Row - TargetEnemyCell->Row) + FMath::Abs(Cell->Column - TargetEnemyCell->Column);
        if (Distance < BestDistance)
        {
            BestDistance = Distance;
            BestCell = Cell;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Cella scelta con distanza %d dal nemico."), BestDistance);
    return BestCell;
}

void APAA_GameMode::ResetUnitActions()
{
    //Setting all boolean flags to false
    for (ABaseUnit* Unit : HpUnits) {
        Unit->bHasMoved = false;
        Unit->bHasAttacked = false;
    }
    for (ABaseUnit* Unit : AiUnits) {
        Unit->bHasMoved = false;
        Unit->bHasAttacked = false;
    }
    //setting counter based on remaining unit
    if (bCurrentPlayer) {
        UnitActions = 2 * HpUnits.Num();
    }
    else {
        UnitActions = 2 * AiUnits.Num();
    }
}
bool APAA_GameMode::UnitActionsFinished(bool bPlayer)
{
    //Checks unit actions counter
    if (UnitActions > 0) {
        return false;
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("actions finished"));
        return true;
    }

}
void APAA_GameMode::GameOver(bool Winner)
{
    //Displaying game over text for winning player
    if (Winner) {
        UE_LOG(LogTemp, Warning, TEXT("Human WINS"));
        UI->AddLogEntry("Human WINS");
        UI->DisplayGameOver(FText::FromString("YOU WIN!"));
        UGameplayStatics::SetGamePaused(GetWorld(), true); //Pausing game ticks
    }
    else if (!Winner) {
        UE_LOG(LogTemp, Warning, TEXT("AI WINS"));
        UI->AddLogEntry("AI WINS");
        UI->DisplayGameOver(FText::FromString("YOU LOSE!"));
        UGameplayStatics::SetGamePaused(GetWorld(), true);
    }
}
void APAA_GameMode::DrawOutcome()
{ //Displaying game over text for draw outcome
    UE_LOG(LogTemp, Warning, TEXT("DRAW"));
    UI->AddLogEntry("DRAW");
    UI->DisplayGameOver(FText::FromString("IT'S A DRAW!"));
    UGameplayStatics::SetGamePaused(GetWorld(), true);
}
void APAA_GameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    //Setup phase
    if (CurrentPhase == EGamePhase::Setup)
    {

        if (bCurrentPlayer && !SetupEnd())
        {

            if (ClickedCell && !ClickedCell->bOccupied)
            {
                if (HpUnits.Num() == 0) {
                    //Placing ally brawler
                    PlaceUnit(ClickedCell, true, true);
                    UI->HideSwitchFirstUnit(); //hiding switch unit button
                    NextTurn();
                }
                else if (HpUnits.Num() == 1)
                { //placing ally sniper
                    PlaceUnit(ClickedCell, true, false);
                    NextTurn();
                }
            }
        }
        else if (!bCurrentPlayer && !SetupEnd())
        {

            int32 RandCell;
            if (GameGrid) {
                //Selecting a random position
                do
                {
                    RandCell = FMath::RandRange(0, GameGrid->GridMatrix.Num() - 1);
                } while (GameGrid->GridMatrix[RandCell]->bIsObstacle || GameGrid->GridMatrix[RandCell]->bOccupied);

                if (AiUnits.Num() == 0) {
                    //placing enemy brawler
                    PlaceUnit(GameGrid->GridMatrix[RandCell], false, true);
                    NextTurn();
                }
                else if (AiUnits.Num() == 1)
                {//placing enemy sniper
                    PlaceUnit(GameGrid->GridMatrix[RandCell], false, false);
                    NextTurn();
                }
            }
        }
        if (SetupEnd())
        {
            //Passing to combat at the end of setup
            CurrentPhase = EGamePhase::Combat;
        }
    }
    //Combat phase
    if (CurrentPhase == EGamePhase::Combat)
    {
        if (bCurrentPlayer) {
            if (ClickedUnit) {
                if (!ClickedUnit->bHasMoved)
                {
                    //Showing movement range
                    ShowMovementRange(ClickedUnit);
                }
                if (!ClickedUnit->bHasAttacked)
                {
                    //Showing enemy units in range
                    ShowAttackRange(ClickedUnit);
                }
            }
            if (UnitActionsFinished(true)) {
                //Waiting for counter to get down to zero before calling next turn
                    if (UnitActions == 0) {
                        NextTurn();
                    }
            }
        }
        else if (!bCurrentPlayer) {
            //Executing ai turn
            ExecuteAiTurn();

        }
        if (HpUnits.IsEmpty() && AiUnits.IsEmpty()) {
            //If both teams are dead calls draw
            DrawOutcome();
        }
        if (HpUnits.IsEmpty()) {
            GameOver(false);
        }
        else if (AiUnits.IsEmpty()) {
            GameOver(true);
        }
    }
}

void APAA_GameMode::ExecuteAiTurn() {
    //Checking for remaining actions
    bool bActionsLeft = UnitActionsFinished(false);
    if (!bActionsLeft) {
        for (ABaseUnit* Unit : AiUnits) {
            if (!Unit->bHasMoved) {
               //Moving enemy unit to closest unit
                ShowMovementRange(Unit);
                Unit->MoveToCell(MoveTowardsEnemy(Unit));
                WipeMovement();
            }
            if (Unit->bHasMoved && !Unit->bHasAttacked) {
                // Getting units in range
                ShowAttackRange(Unit);
                if (AttackableUnits.Num() != 0) {
                    int32 Target = 0;
                    int32 MinHp = 40;
                    for (int i = 0; i < AttackableUnits.Num(); i++) {
                        if (AttackableUnits[i]->OccupyingUnit->HitPoints < MinHp) {
                            MinHp = AttackableUnits[i]->OccupyingUnit->HitPoints;
                            Target = i;
                        }
                    }
                    //Attacks the unit with the lowest hp remaining
                    Unit->AttackUnit(AttackableUnits[Target]->OccupyingUnit);

                }
                WipeTargets();
            }
        }
    }

    else {;
        if (UnitActions == 0) {
            //Waiting for counter to go to zero before calling next turn
            NextTurn();
        }
    }
}
void APAA_GameMode::SetupTopDownCamera()
{
    if (!GameGrid)
    {
        UE_LOG(LogTemp, Warning, TEXT("Impossibile impostare la camera: GameGrid non ancora spawnata."));
        return;
    }

    //setting spawn parameters
    FVector CameraLocation(0.f, 0.f, 2300.f);
    FRotator CameraRotation(-90.f, 0.f, 0.f);
    FActorSpawnParameters CameraSpawnParams;
    TopDownCamera = GetWorld()->SpawnActor<ACameraActor>(CameraLocation, CameraRotation, CameraSpawnParams);
    if (TopDownCamera)
    {
        UE_LOG(LogTemp, Warning, TEXT("CAMERA ACTOR SPAWNATO CON SUCCESSO"));
        // Setting player controller view
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            PC->SetViewTarget(Cast<AActor>(TopDownCamera));
            //Enabling mouse events
            PC->bShowMouseCursor = true;
            PC->bEnableMouseOverEvents = true;
            PC->bEnableClickEvents = true;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Non è stato possibile spawnare la TopDownCamera."));
    }
}

