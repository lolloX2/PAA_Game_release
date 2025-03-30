// Fill out your copyright notice in the Description page of Project Settings.


#include "Sniper.h"

ASniper::ASniper()
{
    // Setting sniper values
    Movement = 3;              
    AttackType = EAttackType::Ranged;  
    AttackRange = 10;             
    DamageMin = 4;              
    DamageMax = 8;             
    HitPoints = 20;           
}
