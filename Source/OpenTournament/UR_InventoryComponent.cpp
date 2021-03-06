// Copyright (c) 2019-2020 Open Tournament Project, All Rights Reserved.

/////////////////////////////////////////////////////////////////////////////////////////////////

#include "UR_InventoryComponent.h"

#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

#include "OpenTournament.h"
#include "UR_Weapon.h"
#include "UR_Ammo.h"
#include "UR_Character.h"

/////////////////////////////////////////////////////////////////////////////////////////////////

UUR_InventoryComponent::UUR_InventoryComponent()
{
    SetIsReplicatedByDefault(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void UUR_InventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(UUR_InventoryComponent, InventoryW, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(UUR_InventoryComponent, DesiredWeapon, COND_SkipOwner);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool UUR_InventoryComponent::IsLocallyControlled() const
{
    APawn* P = Cast<APawn>(GetOwner());
    return P && P->IsLocallyControlled();
}

void UUR_InventoryComponent::Add(AUR_Weapon* InWeapon)
{
    if (InventoryW.Contains(InWeapon))
    {
        // If we already have this weapon instance... there is a logic error
        UE_LOG(LogTemp, Warning, TEXT("%s: weapon instance is already in inventory..."), *GetName());
        return;
    }

    // If we already have this weapon class, just stack ammo
    for (AUR_Weapon* IterWeapon : InventoryW)
    {
        if (IterWeapon->GetClass() == InWeapon->GetClass())
        {
            const int32 NewAmmoCount = IterWeapon->AmmoCount + InWeapon->AmmoCount;
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%s ammo count %i -> %i"), *IterWeapon->WeaponName, IterWeapon->AmmoCount, NewAmmoCount));
            IterWeapon->AmmoCount = NewAmmoCount;
            InWeapon->Destroy();
            return;
        }
    }

    // Else, add weapon
    InventoryW.Add(InWeapon);
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("You have the %s (ammo = %i)"), *InWeapon->WeaponName, InWeapon->AmmoCount));

    // In standalone or listen host, call OnRep next tick so we can pick amongst new weapons what to swap to.
    if (IsLocallyControlled())
    {
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UUR_InventoryComponent::OnRep_InventoryW);
    }
}

void UUR_InventoryComponent::Add(AUR_Ammo* InAmmo)
{
    /*if (InventoryA.Contains(ammo)) {
        for (auto& ammo2 : InventoryA)
        {
            if (ammo2->AmmoName == *ammo->AmmoName) {
                ammo2->amount += ammo->amount;
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("Added %s ammo. Current amount: %d"), *ammo2->AmmoName, ammo2->amount));
            }
        }
    }
    else {*/
        //InventoryA.Add(ammo);
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("You picked the %s"), *ammo->AmmoName));
    //}
        //UpdateWeaponAmmo(ammo);
}

void UUR_InventoryComponent::AmmoCountInInventory(AUR_Weapon* InWeapon) 
{
    for (auto& IterAmmo : InventoryA)
    {
        /*if (weapon->AmmoName == *ammo->AmmoName)
        {
            weapon->AmmoCount += ammo->amount;
        }*/
    }
}

void UUR_InventoryComponent::UpdateWeaponAmmo(AUR_Ammo* InAmmo) 
{
    for (auto& IterWeapon : InventoryW)
    {
        /*if (weapon->AmmoName == *ammo->AmmoName)
        {
            weapon->AmmoCount += ammo->amount;
        }*/
    }
}


void UUR_InventoryComponent::ShowInventory()
{
    for (auto& IterWeapon : InventoryW)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Weapons in inventory: %s with Ammo Count: %d"), *IterWeapon->WeaponName, IterWeapon->AmmoCount));
    }

    for (auto& IterAmmo : InventoryA)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Ammo in inventory: %s"), *ammo->AmmoName));
    }

}

int32 UUR_InventoryComponent::SelectWeapon(int32 WeaponGroup)
{
    FString DesiredWeaponName = (TArray<FString> {
        TEXT("Assault Rifle"),
        TEXT("Shotgun"),
        TEXT("Rocket Launcher"),
        TEXT("Grenade Launcher"),
        TEXT("Sniper Rifle"),
        TEXT("Pistol"),
    })[WeaponGroup];

    for (auto& IterWeapon : InventoryW)
    {
        if (IterWeapon->WeaponName == DesiredWeaponName)
        {
            SetDesiredWeapon(IterWeapon);
            return WeaponGroup;
        }
    }
    return 0;
}

AUR_Weapon * UUR_InventoryComponent::SelectWeaponG(int32 WeaponGroup)
{
    FString DesiredWeaponName = (TArray<FString> {
        TEXT("Assault Rifle"),
        TEXT("Shotgun"),
        TEXT("Rocket Launcher"),
        TEXT("Grenade Launcher"),
        TEXT("Sniper Rifle"),
        TEXT("Pistol"),
    })[WeaponGroup];

    for (auto& IterWeapon : InventoryW)
    {
        if (IterWeapon->WeaponName == DesiredWeaponName)
        {
            SetDesiredWeapon(IterWeapon);
            break;
        }
    }
    return ActiveWeapon;
}

bool UUR_InventoryComponent::NextWeapon()
{
    AUR_Weapon* NewWeapon = nullptr;

    for (int32 i = 0; i < InventoryW.Num(); i++)
    {
        if (InventoryW[i] == DesiredWeapon)
            NewWeapon = InventoryW[(i + 1) % InventoryW.Num()];
    }
    if (!NewWeapon && InventoryW.Num() > 0)
        NewWeapon = InventoryW[0];

    if (NewWeapon && NewWeapon != DesiredWeapon)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Next weapon -> %s"), *NewWeapon->WeaponName));
        SetDesiredWeapon(NewWeapon);
        return true;
    }
    return false;
}

bool UUR_InventoryComponent::PrevWeapon()
{
    AUR_Weapon* NewWeapon = nullptr;

    for (int32 i = 0; i < InventoryW.Num(); i++)
    {
        if (InventoryW[i] == DesiredWeapon)
            NewWeapon = InventoryW[(i + InventoryW.Num() - 1) % InventoryW.Num()];
    }
    if (!NewWeapon && InventoryW.Num() > 0)
        NewWeapon = InventoryW.Last();

    if (NewWeapon && NewWeapon != DesiredWeapon)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Prev weapon -> %s"), *NewWeapon->WeaponName));
        SetDesiredWeapon(NewWeapon);
        return true;
    }
    return false;
}

void UUR_InventoryComponent::SetDesiredWeapon(AUR_Weapon* InWeapon)
{
    DesiredWeapon = InWeapon;

    if (ActiveWeapon && ActiveWeapon->WeaponState != EWeaponState::Inactive)
    {
        if ( ActiveWeapon != DesiredWeapon )
        {
            ActiveWeapon->RequestPutDown();
        }
        else
        {
            ActiveWeapon->RequestBringUp();
        }
    }
    else
    {
        SetActiveWeapon(DesiredWeapon);
    }

    if (GetNetMode() == NM_Client && IsLocallyControlled())
    {
        ServerSetDesiredWeapon(InWeapon);
    }
}

void UUR_InventoryComponent::ServerSetDesiredWeapon_Implementation(AUR_Weapon* InWeapon)
{
    // On server
    SetDesiredWeapon(InWeapon);
}

void UUR_InventoryComponent::OnRep_DesiredWeapon()
{
    // On remote clients
    SetDesiredWeapon(DesiredWeapon);
}

void UUR_InventoryComponent::OnActiveWeaponStateChanged(AUR_Weapon* Weapon, EWeaponState NewState)
{
    if (Weapon && Weapon == ActiveWeapon && NewState == EWeaponState::Inactive)
    {
        SetActiveWeapon(DesiredWeapon);
    }
}

void UUR_InventoryComponent::SetActiveWeapon(AUR_Weapon* InWeapon)
{
    if (ActiveWeapon)
    {
        ActiveWeapon->OnWeaponStateChanged.RemoveDynamic(this, &UUR_InventoryComponent::OnActiveWeaponStateChanged);

        // Edge case - eg. weapondrop doesn't go through the putdown procedure.
        if (ActiveWeapon->WeaponState != EWeaponState::Inactive)
        {
            // this ensures the mesh is detached from pawn
            ActiveWeapon->SetWeaponState(EWeaponState::Inactive);
        }

        ActiveWeapon = NULL;
    }

    //NOTE: InventoryW is not replicated for non-owning clients
    if (InWeapon && (GetNetMode() == NM_Client || InventoryW.Contains(InWeapon)))
    {
        ActiveWeapon = InWeapon;
        ActiveWeapon->RequestBringUp();
        ActiveWeapon->OnWeaponStateChanged.AddUniqueDynamic(this, &UUR_InventoryComponent::OnActiveWeaponStateChanged);
    }

    //modsupport - case where SetActiveWeapon is called without setting DesiredWeapon.
    // We need to replicate to remote clients via DesiredWeapon.
    if (GetNetMode() != NM_Client)
    {
        DesiredWeapon = ActiveWeapon;
    }
}

void UUR_InventoryComponent::OnRep_InventoryW()
{
    if (!ActiveWeapon)
    {
        // This should only happen when we are given initial inventory on spawn
        // Here we should use user settings to pick the preferred weapon (if there are multiple).
        for (AUR_Weapon* IterWeapon : InventoryW)
        {
            if (IterWeapon)
            {
                SetDesiredWeapon(IterWeapon);
                break;
            }
        }
    }
}

void UUR_InventoryComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
    // !IsUnreachable() avoids crash during endgame world cleanup, trying to resolve bp-enabled events
    if (GetOwnerRole() == ROLE_Authority && !IsUnreachable())
    {
        //TODO: drop active weapon

        Clear();
    }

    Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UUR_InventoryComponent::Clear_Implementation()
{
    for (AUR_Weapon* IterWeapon : InventoryW)
    {
        if (IterWeapon)
            IterWeapon->Destroy();
    }
    InventoryW.Empty();

    for (AUR_Ammo* IterAmmo : InventoryA)
    {
        if (IterAmmo)
        {
            IterAmmo->Destroy();
        }
    }
    InventoryA.Empty();
}
