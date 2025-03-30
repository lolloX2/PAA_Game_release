// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "UserInterface.generated.h"


class ABaseUnit;

UCLASS()
class PAA_GAME_API UUserInterface : public UUserWidget
{
	GENERATED_BODY()
public:
	//Current player text box
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* Turn;
	//Move history text box
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MoveLog_Text;
	//Move history scrollbox
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* MoveLog;
	//Health bars
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HB_HealthBar;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HS_HealthBar;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* AIB_HealthBar;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* AIS_HealthBar;
	UPROPERTY(meta = (BindWidget))
	//Remaining HP text boxes
	class UTextBlock* HB_Health;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HS_Health;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AIB_Health;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AIS_Health;
	//Game over text
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GameOver;
	//Pass Button (onclick logic managed in BP)
	UPROPERTY(meta = (BindWidget))
	UButton* Pass_Btn;
	//Switch unit button (onclick logic managed in BP)
	UPROPERTY(meta = (BindWidget))
	UButton* SwitchFirstUnit;
	//Brawler icon
	UPROPERTY(meta = (BindWidget))
	class UImage* B_Icon;
	//Sniper icon
	UPROPERTY(meta = (BindWidget))
	class UImage* S_Icon;
	//Quit Button
	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;
	
	//UI functions
	void SetFirstUnitText(bool SniperFirst);
	void HideSwitchFirstUnit();
	void SetTurnText(FText TurnText);
	void AddLogEntry(FString LogEntry);
	void DisplayHealth(ABaseUnit* Unit, int32 Health);
	void DisplayGameOver(FText Outcome);
};
