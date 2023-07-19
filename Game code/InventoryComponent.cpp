#include "Components/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
// Needed for replicating the UObjs
#include "Engine/ActorChannel.h"

#define LOCTEXT_NAMESPACE "Inventory"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	SetIsReplicatedByDefault(true);
}


FItemAddResult UInventoryComponent::TryAddItem(class UItem* Item)
{
	return TryAddItem_Internal(Item);
}

FItemAddResult UInventoryComponent::TryAddItemFromClass(TSubclassOf<class UItem> ItemClass, const int32 Quantity)
{
	UItem* Item = NewObject<UItem>(GetOwner(), ItemClass);
	Item->SetQuantity(Quantity);
	return TryAddItem_Internal(Item);
}

int32 UInventoryComponent::ConsumeItem(class UItem* Item)
{
	if (Item)
	{
		ConsumeItem(Item, Item->GetQuantity());
	}

	return 0;
}

int32 UInventoryComponent::ConsumeItem(class UItem* Item, const int32 Quantity)
{
	// If we're the Server
	if (GetOwner() && GetOwner()->HasAuthority() && Item)
	{
		const int32 RemoveQuantity = FMath::Min(Quantity, Item->GetQuantity());

		// We shouldn't have a negative amount of the item after the drop
		ensure(!(Item->GetQuantity() - RemoveQuantity < 0));

		// We now have 0 of this item, remove it from the inventory
		Item->SetQuantity(Item->GetQuantity() - RemoveQuantity);

		if (Item->GetQuantity() <= 0)
		{
			RemoveItem(Item);
		}
		else
		{
			ClientRefreshInventory();
		}

		return RemoveQuantity;
	}

	return 0;
}

bool UInventoryComponent::RemoveItem(class UItem* Item)
{
	// Make sure the owner of the Inventory Comp is the Server
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		if (Item)
		{
			Items.RemoveSingle(Item);

			OnRep_Items();

			ReplicatedItemsKey++;

			return true;
		}
	}

	return false;
}

bool UInventoryComponent::HasItem(TSubclassOf<class UItem> ItemClass, const int32 Quantity /*= 1*/) const
{
	if (UItem* ItemToFind = FindItemByClass(ItemClass))
	{
		return ItemToFind->GetQuantity() >= Quantity;
	}

	return false;
}

UItem* UInventoryComponent::FindItem(class UItem* Item) const
{
	if (Item)
	{
		for (auto& InventoryItem : Items)
		{
			if (InventoryItem && InventoryItem->GetClass() == Item->GetClass())
			{
				return InventoryItem;
			}
		}
	}

	return nullptr;
}

UItem* UInventoryComponent::FindItemByClass(TSubclassOf<class UItem> ItemClass) const
{
	for (auto& InventoryItem : Items)
	{
		if (InventoryItem && InventoryItem->GetClass() == ItemClass)
		{
			return InventoryItem;
		}
	}

	return nullptr;
}

TArray<UItem*> UInventoryComponent::FindItemsByClass(TSubclassOf<class UItem> ItemClass) const
{
	TArray<UItem*> ItemsOfClass;

	for (auto& InventoryItem : Items)
	{
		if (InventoryItem && InventoryItem->GetClass()->IsChildOf(ItemClass))
		{
			ItemsOfClass.Add(InventoryItem);
		}
	}

	return ItemsOfClass;
}

float UInventoryComponent::GetCurrentWeight() const
{
	float Weight = 0.f;

	for (auto& Item : Items)
	{
		if (Item)
		{
			Weight += Item->GetStackWeight();
		}
	}

	return Weight;
}

void UInventoryComponent::SetWeightCapacity(const float NewWeightCapacity)
{
	WeightCapacity = NewWeightCapacity;
	// Calling the delegate to update the UI
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::SetCapacity(const int32 NewCapacity)
{
	Capacity = NewCapacity;
	// Calling the delegate to update the UI
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::ClientRefreshInventory_Implementation()
{
	OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// All we want to replicate is the items
	DOREPLIFETIME(UInventoryComponent, Items);
}

// Where we do the replication of our UObjs
bool UInventoryComponent::ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	// Call the Super fnc b/c there's other stuff that the UE class needs to do before we can override its functionality
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	// Check if the array of items needs to replicate using the key
	if (Channel->KeyNeedsToReplicate(0, ReplicatedItemsKey))
	{
		// Iterate thru all of our items
		for (auto& Item : Items)
		{
			// Check if the items RepKey returns true (meaning it needs to be replicated)
			if (Channel->KeyNeedsToReplicate(Item->GetUniqueID(), Item->RepKey))
			{
				bWroteSomething |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
			}
		}
	}

	return bWroteSomething;
}

UItem* UInventoryComponent::AddItem(class UItem* Item)
{
	// If the owner is the Server
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		// To guarantee that the owner of the item is the Inventory Comp, we'll reconstruct the item (basically duplicating the item)
		UItem* NewItem = NewObject<UItem>(GetOwner(), Item->GetClass());
		NewItem->World = GetWorld();
		NewItem->SetQuantity(Item->GetQuantity());
		NewItem->OwningInventory = this;
		NewItem->AddedToInventory(this);
		Items.Add(NewItem);
		NewItem->MarkDirtyForReplication();

		return NewItem;
	}

	return nullptr;
}

void UInventoryComponent::OnRep_Items()
{
	OnInventoryUpdated.Broadcast();

	for (auto& Item : Items)
	{
		// If the item exists + is not in the World
		if (Item && !Item->World)
		{
			// Tell the item about its World
			Item->World = GetWorld();
		}
	}
}

// Our workhorse fnc
FItemAddResult UInventoryComponent::TryAddItem_Internal(class UItem* Item)
{
	// If we're the Server
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		const int32 AddAmount = Item->GetQuantity();

		if (Items.Num() + 1 > GetCapacity())
		{
			return FItemAddResult::AddedNone(AddAmount, LOCTEXT("InventoryCapacityFullText", "Couldn't add item to Inventory. Inventory is full"));
		}

		// Items w/ a weight of zero don't require a weight check
		if (!FMath::IsNearlyZero(Item->Weight))
		{
			if (GetCurrentWeight() + Item->Weight > GetWeightCapacity())
			{
				return FItemAddResult::AddedNone(AddAmount, LOCTEXT("InventoryTooMuchWeightText", "Couldn't add item to Inventory. Carrying too much weight"));
			}
		}

		// If the item is stackable, check if we already have it + add it to their stack
		if (Item->bStackable)
		{
			// Somehow the items quantity went over the max stack size. This shouldn't ever happen
			ensure(Item->GetQuantity() <= Item->MaxStackSize);

			if (UItem* ExistingItem = FindItem(Item))
			{
				if (ExistingItem->GetQuantity() < ExistingItem->MaxStackSize)
				{
					// Find out how much of the item to add
					const int32 CapacityMaxAddAmount = ExistingItem->MaxStackSize - ExistingItem->GetQuantity();
					int32 ActualAddAmount = FMath::Min(AddAmount, CapacityMaxAddAmount);

					FText ErrorText = LOCTEXT("InventoryErrorText", "Couldn't add all of the item to your inventory");

					// Adjust based on how much weight we can carry
					if (!FMath::IsNearlyZero(Item->Weight))
					{
						// Find the max amount of the item we could take due to weight
						const int32 WeightMaxAddAmount = FMath::FloorToInt((WeightCapacity - GetCurrentWeight()) / Item->Weight);
						ActualAddAmount = FMath::Min(ActualAddAmount, WeightMaxAddAmount);

						if (ActualAddAmount < AddAmount)
						{
							ErrorText = FText::Format(LOCTEXT("InventoryTooMuchWeightText", "Couldn't add entire stack of {ItemName} to Inventory."), Item->ItemDisplayName);
						}
					}
					else if (ActualAddAmount < AddAmount)
					{
						// If the item weighs none + we can't take it, then there was a capacity issue
						ErrorText = FText::Format(LOCTEXT("InventoryCapacityFullText", "Couldn't add entire stack of {ItemName} to Inventory. Inventory was full."), Item->ItemDisplayName);
					}

					// We couldn't add any of the item to our inventory
					if (ActualAddAmount <= 0)
					{
						return FItemAddResult::AddedNone(AddAmount, LOCTEXT("InventoryErrorText", "Couldn't add item to inventory."));
					}

					ExistingItem->SetQuantity(ExistingItem->GetQuantity() + ActualAddAmount);

					// If we somehow get more of the item than the max stack size then something is wrong w/ our math
					ensure(ExistingItem->GetQuantity() <= ExistingItem->MaxStackSize);

					if (ActualAddAmount < AddAmount)
					{
						return FItemAddResult::AddedSome(AddAmount, ActualAddAmount, ErrorText);
					}
					else
					{
						return FItemAddResult::AddedAll(AddAmount);
					}
				}
				else
				{
					return FItemAddResult::AddedNone(AddAmount, FText::Format(LOCTEXT("InventoryFullStackText", "Couldn't add {ItemName}. You already have a full stack of this item"), Item->ItemDisplayName));
				}
			}
			else
			{
				// Since we don't have any of this item, we'll add the full stack
				AddItem(Item);

				return FItemAddResult::AddedAll(AddAmount);
			}
		}
		else // Item is not stackable
		{
			// Non-stackables should always have a quantity of 1
			ensure(Item->GetQuantity() == 1);

			AddItem(Item);

			return FItemAddResult::AddedAll(AddAmount);
		}
	}

	// AddItem() should never be called on a Client
	check(false);
	return FItemAddResult::AddedNone(-1, LOCTEXT("ErrorMessage", ""));
}

#undef LOCTEXT_NAMESPACE