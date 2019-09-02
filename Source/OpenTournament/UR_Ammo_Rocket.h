// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConstructorHelpers.h"
#include "UR_Ammo.h"
#include "GameFramework/Actor.h"
#include "UR_Ammo_Rocket.generated.h"


/**
 *
 */

class AUR_Ammo_Rocket;

UCLASS()
class OPENTOURNAMENT_API AUR_Ammo_Rocket : public AUR_Ammo
{
	GENERATED_BODY()

	// Sets default values for this actor's properties
	AUR_Ammo_Rocket(const FObjectInitializer& ObjectInitializer);

};