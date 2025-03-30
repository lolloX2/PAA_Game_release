// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "BaseUnit.h"
#include "UserInterface.h"



void UUserInterface::SetFirstUnitText(bool SniperFirst)
{
    if (B_Icon && S_Icon) {
        if (SniperFirst) {
            //Displaying sniper icon for placing sniper
            B_Icon->SetVisibility(ESlateVisibility::Collapsed);
            S_Icon->SetVisibility(ESlateVisibility::Visible);
        }
        else {
            //Displaying brawler icon for placing brawler
            B_Icon->SetVisibility(ESlateVisibility::Visible);
            S_Icon->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void UUserInterface::HideSwitchFirstUnit()
{
    //Hides unit switch button after first placement
    SwitchFirstUnit->SetVisibility(ESlateVisibility::Collapsed);
}

void UUserInterface::SetTurnText(FText TurnText)
{
    if (Turn) {
        //Displays current player
        Turn->SetText(TurnText);
    }
    else return;
}

void UUserInterface::AddLogEntry(FString LogEntry)
{
    if (MoveLog_Text)
    {
        //Appendig move to move history
        FString CurrentText = MoveLog_Text->GetText().ToString();
        CurrentText.Append("\n");
        CurrentText.Append(LogEntry);

        MoveLog_Text->SetText(FText::FromString(CurrentText));
       

        if (MoveLog) {
           //Scrolling to the bottom
            MoveLog->ScrollToEnd();
        }
    } 
}

void UUserInterface::DisplayHealth(ABaseUnit* Unit, int32 Health)
{
    if (Unit) {
        float HealthPercent = 0.f;
        if (Unit->bTeam) {
            if (Unit->AttackType == EAttackType::Melee) {
                if (HB_Health) {
                    //Setting remaining HP text
                    HB_Health->SetText(FText::FromString(FString::Printf(TEXT("%d/40"), Health)));
                }
                if (HB_HealthBar){
                    //Setting healthbar
                    HealthPercent = static_cast<float>(Health) / 40.f;
                    HB_HealthBar->SetPercent(HealthPercent);
                }
            }
            else {
                if (HS_Health) {
                    HS_Health->SetText(FText::FromString(FString::Printf(TEXT("%d/20"), Health)));
                }
                if (HS_HealthBar) {
                    HealthPercent = static_cast<float>(Health) / 20.f;
                    HS_HealthBar->SetPercent(HealthPercent);
                }
            }
        }
        else {
            if (Unit->AttackType == EAttackType::Melee) {
                if (AIB_Health) {
                    AIB_Health->SetText(FText::FromString(FString::Printf(TEXT("%d/40"), Health)));
                }
                if (AIB_HealthBar) {
                    HealthPercent = static_cast<float>(Health) / 40.f;
                    AIB_HealthBar->SetPercent(HealthPercent);
                }
            }
            else {
                if (AIS_Health) {
                    AIS_Health->SetText(FText::FromString(FString::Printf(TEXT("%d/20"), Health)));
                }
                if (AIS_HealthBar) {
                    HealthPercent = static_cast<float>(Health) / 20.f;
                    AIS_HealthBar->SetPercent(HealthPercent);
                }
            }
        }
    }

}

void UUserInterface::DisplayGameOver(FText Outcome)
{
    if (GameOver) {
        //Displays game over text and quit button
        GameOver->SetVisibility(ESlateVisibility::Visible);
        GameOver->SetText(Outcome);
        QuitButton->SetVisibility(ESlateVisibility::Visible);
    }
}
