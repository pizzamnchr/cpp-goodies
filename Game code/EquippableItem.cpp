#include "Items/EquippableItem.h"
#include "Net/UnrealNetwork.h"
#include "Player/SurvivalCharacter.h"
#include "Components/InventoryComponent.h"

#define LOCTEXT_NAMESPACE "EquippableItem"

UEquippableItem::UEquippableItem()
{
	bStackable = false;
	bEquipped = false;
	UseActionText = LOCTEXT("ItemUseActionText", "Equip");
}

void UEquippableItem::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEquippableItem, bEquipped);
}

void UEquippableItem::Use(class ASurvivalCharacter* Character)
{
	// If the Character is on the Server
	if (Character && Character->HasAuthority())
	{
		// Check if the item is at a slot + isn't equipped
		if (Character->GetEquippedItems().Contains(Slot) && !bEquipped)
		{
			UEquippableItem* AlreadyEquippedItem = *Character->GetEquippedItems().Find(Slot);
			
			AlreadyEquippedItem->SetEquipped(false);
		}

		// If the item's equipped, unequip it, + if it isn't, equip it
		SetEquipped(!IsEquipped());
	}
}

bool UEquippableItem::Equip(class ASurvivalCharacter* Character)
{
	if (Character)
	{
		return Character->EquipItem(this);
	}

	return false;
}

bool UEquippableItem::UnEquip(class ASurvivalCharacter* Character)
{
	if (Character)
	{
		return Character->UnEquipItem(this);
	}

	return false;
}

bool UEquippableItem::ShouldShowInInventory() const
{
	return !bEquipped;
}

void UEquippableItem::AddedToInventory(class UInventoryComponent* Inventory)
{
	// If the player looted an item, don't equip it
	if (ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(Inventory->GetOwner()))
	{
		if (Character && !Character->IsLooting())
		{
			// If we take an equippable + don't have an item already equipped at its slot, then auto equip it
			if (!Character->GetEquippedItems().Contains(Slot))
			{
				SetEquipped(true);
			}
		}
	}
}

void UEquippableItem::SetEquipped(bool bNewEquipped)
{
	bEquipped = bNewEquipped;
	EquipStatusChanged();
	MarkDirtyForReplication();
}

void UEquippableItem::EquipStatusChanged()
{
	if (ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(GetOuter()))
	{
		if (bEquipped)
		{
			Equip(Character);
		}
		else
		{
			UnEquip(Character);
		}
	}

	// Telling the UI to update
	OnItemModified.Broadcast();
}

#undef LOCTEXT_NAMESPACE 