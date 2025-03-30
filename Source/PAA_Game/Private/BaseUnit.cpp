// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseUnit.h"
#include "PAA_GameMode.h"
// Sets default values
ABaseUnit::ABaseUnit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Creating and setting the mesh as root component
	UnitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnitMesh"));
	RootComponent = UnitMesh;

	//Setting unit as a Brawler by default
	Movement = 0; 
	AttackType = EAttackType::Melee;
	AttackRange = 0; 
	DamageMin = 0;
	DamageMax = 1;
	HitPoints = 1;
	UnitMesh->OnClicked.AddDynamic(this, &ABaseUnit::OnUnitClicked);
	bIsMoving = false;
	MovementTarget = FVector::ZeroVector;
	MovementSpeed = 300.f;

}


// Called when the game starts or when spawned
void ABaseUnit::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	{
		Super::Tick(DeltaTime);

		if (bIsMoving && Waypoints.Num() > 0)
		{
			FVector CurrentLocation = GetActorLocation();
			FVector TargetWaypoint = Waypoints[0];

			// Calculates a new position towards the waypoint with constant speed
			FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetWaypoint, DeltaTime, MovementSpeed);
			SetActorLocation(NewLocation);

			// If the unit is close enough to the current waypoint, it gets removed an pass to the next one
			if (FVector::Dist(NewLocation, TargetWaypoint) < 10.f)
			{
				Waypoints.RemoveAt(0);
				if (Waypoints.Num() == 0)
				{
					APAA_GameMode* GM = Cast<APAA_GameMode>(GetWorld()->GetAuthGameMode());
					if (GM)
					{
						GM->UnitActions--; //Action counter updated at the end of the movement animation to time the turns correctly
					}
					bIsMoving = false; // end of path
				}
			}
		}
	}
}

int32 ABaseUnit::CalculateDamage() const
{
	return FMath::RandRange(DamageMin, DamageMax);
}

void ABaseUnit::ReceiveDamage(int32 DamageAmount)
{
	HitPoints -= DamageAmount;
	if (HitPoints < 0)
	{
		HitPoints = 0;
	}
	// Managing a unit's death
	if (!IsAlive()) {
		APAA_GameMode* GM = Cast<APAA_GameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			if (bTeam) {
				// Unassigning pointers of dead unit
				OccupiedCell->bOccupied = false;
				OccupiedCell->OccupyingUnit = nullptr;
				OccupiedCell = nullptr;
				// If a unit dies in their turn is because of counterattack, we need to update correctly the unit actions counter
				if (GM->bCurrentPlayer) {
					// if the unit hasn't moved yet we need to reduce the counter
					if (!bHasMoved) {						
						GM->UnitActions--;
					}
				}
				GM->HpUnits.Remove(this);
				Destroy();
			}
			else if (!bTeam) {
				OccupiedCell->bOccupied = false;
				OccupiedCell->OccupyingUnit = nullptr;
				OccupiedCell = nullptr;
				if (!GM->bCurrentPlayer) {
					if (!bHasMoved) {						
						GM->UnitActions--;
					}
				}
				GM->AiUnits.Remove(this);
				Destroy();
			}
		}
	}
}

int32 ABaseUnit::CalculateCounterAttack()
{
	return FMath::RandRange(1, 3);
}

void ABaseUnit::AttackUnit(ABaseUnit* TargetUnit)
{
	//Calculating damage
	int32 Damage = CalculateDamage();
	ACell* TargetCell = TargetUnit->OccupiedCell;
	// Applying damage to target unit
	TargetUnit->ReceiveDamage(Damage);
	//Checking for counterattack
	if (AttackType == EAttackType::Ranged) {
		int32 CounterDmg = CalculateCounterAttack();
		// Sniper attacks enemy sniper
		if (TargetUnit->AttackType == EAttackType::Ranged) {
			ReceiveDamage(CounterDmg);
		}
		//Sniper attacks enemy brawler at melee distance
		else if (TargetUnit->AttackType == EAttackType::Melee) {
			APAA_GameMode* GM = Cast<APAA_GameMode>(GetWorld()->GetAuthGameMode());
			if (GM) {
				//checking adjacent cells
				TArray<ACell*> AdjacentCells = GM->GetNeighbors(OccupiedCell);
				for (ACell* Cell : AdjacentCells) {
					if (Cell->OccupyingUnit == TargetUnit) {
						ReceiveDamage(CounterDmg);
					}
				}
				
			}
		}
	}
	//Printing attack in move history
	LogUnitAttack(bTeam, this, TargetCell, Damage);
	APAA_GameMode* GM = Cast<APAA_GameMode>(GetWorld()->GetAuthGameMode());
	if (GM) {
		//Update actions counter and healthbars
		GM->UnitActions--;
		GM->UI->DisplayHealth(this, HitPoints);
		GM->UI->DisplayHealth(TargetUnit, TargetUnit->HitPoints);
	}
	bHasAttacked = true;
}

bool ABaseUnit::IsAlive() const
{
	return HitPoints > 0;
}

void ABaseUnit::OnUnitClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	APAA_GameMode* GM = Cast<APAA_GameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		if (GM->HpUnits.Contains(this)) {
			// clearing grid highlights
			GM->WipeMovement();
			GM->WipeTargets();
			GM->ClickedUnit = this; //Sets clicked unit for movement and attack range display
		}
		else if(GM->AiUnits.Contains(this)) {
			if (GM->AttackableUnits.Contains(this->OccupiedCell)) {
				if (GM->ClickedUnit) {
					GM->ClickedUnit->AttackUnit(this); //attacks an enemy unit in range
				}
			}
			//clearing grid highlights at the end of the attack or clicking on a unit out of range
				GM->WipeTargets();
				GM->WipeMovement();
			
			}
		}
	}


void ABaseUnit::MoveToCell(ACell* DestinationCell)
{
	if (!DestinationCell)
		return;

	// Rearranging starting cell pointers
	OccupiedCell->bOccupied = false;
	OccupiedCell->OccupyingUnit = nullptr;
	// Setting world destination
	MovementTarget = DestinationCell->GetActorLocation() + FVector(0.f, 0.f, 5.f);
	bIsMoving = true;
	APAA_GameMode* GM = Cast<APAA_GameMode>(GetWorld()->GetAuthGameMode());
	if (GM) {
		// Calculates realistic movement path
		TArray<ACell*> CalculatedPath = GM->FindPath(OccupiedCell, DestinationCell);
		SetMovementPath(CalculatedPath);
		// Prints movement in move history
		LogUnitMove(bTeam, this, OccupiedCell, DestinationCell);
		//Setting destination cell pointers
		OccupiedCell = DestinationCell;
		OccupiedCell->bOccupied = true;
		OccupiedCell->OccupyingUnit = this;
		
		bHasMoved = true;
		if (!bHasAttacked) {
			// Updating attack flag and actions counter if there are no enemy units in range after moving
				if (GM->GetAttackTargets(OccupiedCell, AttackRange).Num() == 0) {
					GM->UnitActions--;
					bHasAttacked = true;
				}
			
		}
	}
}

void ABaseUnit::SetMovementPath(const TArray<ACell*>& Path)
{
	Waypoints.Empty();
	// Converts each cell in path to a waypoint
	for (ACell* Cell : Path)
	{
		if (Cell)
		{
			FVector Waypoint = Cell->GetActorLocation() + FVector(0.f, 0.f, 5.f);
			Waypoints.Add(Waypoint);
		}
	}

	bIsMoving = Waypoints.Num() > 0;
}

void ABaseUnit::LogUnitMove(bool bPlayer, ABaseUnit* Unit, ACell* From, ACell* To)
{
	const FString PlayerID = bPlayer ? TEXT("HP") : TEXT("AI");
	const FString UnitID = Unit->AttackType == EAttackType::Ranged ? TEXT("S") : TEXT("B");
	const FString FromID = From ? From->GetCellID() : TEXT("");
	const FString ToID = To ? To->GetCellID() : TEXT("");
	UE_LOG(LogTemp, Log, TEXT("%s: %s %s -> %s"), *PlayerID, *UnitID, *FromID, *ToID);
	FString LogEntry = FString::Printf(TEXT("%s: %s %s -> %s"), *PlayerID, *UnitID, *FromID, *ToID);
	APAA_GameMode* GM = Cast<APAA_GameMode>(GetWorld()->GetAuthGameMode());
	if (GM) {
		GM->UI->AddLogEntry(LogEntry);
	}
	
}

void ABaseUnit::LogUnitAttack(bool bPlayer, ABaseUnit* Unit, ACell* TargetCell, int32 Damage)
{
	const FString PlayerID = bPlayer ? TEXT("HP") : TEXT("AI");
	const FString UnitID = Unit->AttackType == EAttackType::Ranged ? TEXT("S") : TEXT("B");
	const FString TargetID = TargetCell ? TargetCell->GetCellID() : TEXT("");
	UE_LOG(LogTemp, Log, TEXT("%s: %s %s %d"), *PlayerID, *UnitID, *TargetID, Damage);
	FString LogEntry = FString::Printf(TEXT("%s: %s %s %d"), *PlayerID, *UnitID, *TargetID, Damage);
	APAA_GameMode* GM = Cast<APAA_GameMode>(GetWorld()->GetAuthGameMode());
	if (GM) {
		GM->UI->AddLogEntry(LogEntry);
	}
}


