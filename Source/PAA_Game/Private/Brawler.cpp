// Fill out your copyright notice in the Description page of Project Settings.


#include "Brawler.h"

ABrawler::ABrawler() 
{
    // Setting brawler values
    Movement = 6;             
    AttackType = EAttackType::Melee;
    AttackRange = 1;            
    DamageMin = 1;             
    DamageMax = 6;              
    HitPoints = 40;             
}