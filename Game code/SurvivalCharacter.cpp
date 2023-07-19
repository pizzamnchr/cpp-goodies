#include "SurvivalCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "Player/SurvivalPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/InteractionComponent.h"
#include "Items/EquippableItem.h"
#include "Items/GearItem.h"
#include "Items/WeaponItem.h"
#include "Items/ThrowableItem.h"
#include "Materials/MaterialInstance.h"
#include "World/Pickup.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "SurvivalGame.h"
#include "Weapons/MeleeDamage.h"
#include "Weapons/ThrowableWeapon.h"
#include "Weapons/Weapon.h"

#define LOCTEXT_NAMESPACE "SurvivalCharacter"

static FName NAME_AimDownSightsSocket("ADSSocket");

// Sets default values
ASurvivalCharacter::ASurvivalCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(GetMesh(), FName("Head"));
	SpringArmComponent->TargetArmLength = 0.f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = true;

	// Body
	HairMesh = PlayerMeshes.Add(EEquippableSlot::EIS_Hair, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HairMesh")));
	ChestMesh = PlayerMeshes.Add(EEquippableSlot::EIS_Chest, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ChestMesh")));
	LeftHandMesh = PlayerMeshes.Add(EEquippableSlot::EIS_LeftHand, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftHandMesh")));
	RightHandMesh = PlayerMeshes.Add(EEquippableSlot::EIS_RightHand, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightHandMesh")));
	LegsMesh = PlayerMeshes.Add(EEquippableSlot::EIS_Legs, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegsMesh")));
	FeetMesh = PlayerMeshes.Add(EEquippableSlot::EIS_Feet, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FeetMesh")));

	// Equippable clothing
	HelmetMesh = PlayerMeshes.Add(EEquippableSlot::EIS_Helmet, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HelmetMesh")));
	CloakMesh = PlayerMeshes.Add(EEquippableSlot::EIS_Cloak, CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CloakMesh")));

	// Tell all the body meshes to use the head mesh for animation
	for (auto& PlayerMesh : PlayerMeshes)
	{
		USkeletalMeshComponent* MeshComponent = PlayerMesh.Value;
		MeshComponent->SetupAttachment(GetMesh());
		MeshComponent->SetMasterPoseComponent(GetMesh());
	}

	// Since we don't want to attach the Head to the Head
	PlayerMeshes.Add(EEquippableSlot::EIS_Head, GetMesh());
	GetMesh()->SetOwnerNoSee(true);
	//HairMesh->SetOwnerNoSee(true);

	// Give the player an inventory w/ 20 slots + an 80kg capacity
	PlayerInventory = CreateDefaultSubobject<UInventoryComponent>("PlayerInventory");
	PlayerInventory->SetCapacity(20);
	PlayerInventory->SetWeightCapacity(80.f);

	LootPlayerInteraction = CreateDefaultSubobject<UInteractionComponent>("PlayerInteraction");
	LootPlayerInteraction->InteractableActionText = LOCTEXT("LootPlayerText", "Loot");
	LootPlayerInteraction->InteractableNameText = LOCTEXT("LootPlayerName", "Player");
	LootPlayerInteraction->SetupAttachment(GetRootComponent());
	LootPlayerInteraction->SetActive(false, true);
	// Don't loot players when they're alive
	LootPlayerInteraction->bAutoActivate = false;

	// 0 = Check every frame
	InteractionCheckFrequency = 0.f;
	// Furthest we'll allow is 10 meters
	InteractionCheckDistance = 1000.f;

	MaxHealth = 100.f;
	Health = MaxHealth;

	MeleeAttackDistance = 150.f;
	MeleeAttackDamage = 20.f;

	SprintSpeed = GetCharacterMovement()->MaxWalkSpeed * 1.3f;
	WalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

	bIsAiming = false;

	//GetMesh()->SetOwnerNoSee(true);
	HairMesh->SetOwnerNoSee(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	SetReplicateMovement(true);
	SetReplicates(true);
	bAlwaysRelevant = true;
}

// Called when the game starts or when spawned
void ASurvivalCharacter::BeginPlay()
{
	Super::BeginPlay();

	LootPlayerInteraction->OnInteract.AddDynamic(this, &ASurvivalCharacter::BeginLootingPlayer);

	// Try to display the players' platform names on their loot cards
	if (APlayerState* PS = GetPlayerState())
	{
		LootPlayerInteraction->SetInteractableNameText(FText::FromString(PS->GetPlayerName()));
	}
	
	// When the player spawns in they have no items equipped, so cache these items (That way, if a player unequips an item we can set the mesh back)
	for (auto& PlayerMesh : PlayerMeshes)
	{
		if (PlayerMesh.Value != NULL)
		{
			NakedMeshes.Add(PlayerMesh.Key, PlayerMesh.Value->SkeletalMesh);
		}
	}
}

void ASurvivalCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASurvivalCharacter, bSprinting);

	DOREPLIFETIME(ASurvivalCharacter, LootSource);

	DOREPLIFETIME(ASurvivalCharacter, EquippedWeapon);

	DOREPLIFETIME(ASurvivalCharacter, Killer);

	// Only the owner needs to know about their health!
	DOREPLIFETIME_CONDITION(ASurvivalCharacter, Health, COND_OwnerOnly);

	// Replicate aiming to everyone else EXCEPT the owner as that'll make aiming feel laggy for the owner
	DOREPLIFETIME_CONDITION(ASurvivalCharacter, bIsAiming, COND_SkipOwner);
}

bool ASurvivalCharacter::IsInteracting() const
{
	// Check if the timer is active
	return GetWorldTimerManager().IsTimerActive(TimerHandle_Interact);
}

float ASurvivalCharacter::GetRemainingInteractTime() const
{
	// Get the time remaining on the handle
	return GetWorldTimerManager().GetTimerRemaining(TimerHandle_Interact);
}

void ASurvivalCharacter::UseItem(class UItem* Item)
{
	// If we're the Client + the Item is valid
	if (!HasAuthority() && Item)
	{
		ServerUseItem(Item);
	}

	// If we're the Server
	if (HasAuthority())
	{
		// If the inventory doesn't have the item
		if (PlayerInventory && !PlayerInventory->FindItem(Item))
		{
			return;
		}
	}

	if (Item)
	{
		Item->OnUse(this);
		Item->Use(this);
	}
}

void ASurvivalCharacter::ServerUseItem_Implementation(class UItem* Item)
{
	UseItem(Item);
}

bool ASurvivalCharacter::ServerUseItem_Validate(class UItem* Item)
{
	return true;
}

void ASurvivalCharacter::DropItem(class UItem* Item, const int32 Quantity)
{
	// If we have the item in our inventory
	if (PlayerInventory && Item && PlayerInventory->FindItem(Item))
	{
		// If we're the Server
		if (!HasAuthority())
		{
			ServerDropItem(Item, Quantity);
			return;
		}

		// If we're the Server
		if (HasAuthority())
		{
			const int32 ItemQuantity = Item->GetQuantity();
			const int32 DroppedQuantity = PlayerInventory->ConsumeItem(Item, Quantity);

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.bNoFail = true;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			FVector SpawnLocation = GetActorLocation();
			SpawnLocation.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

			FTransform SpawnTransform(GetActorRotation(), SpawnLocation);

			ensure(PickupClass);

			if (APickup* Pickup = GetWorld()->SpawnActor<APickup>(PickupClass, SpawnTransform, SpawnParams))
			{
				Pickup->InitializePickup(Item->GetClass(), DroppedQuantity);
			}
		}
	}
}

void ASurvivalCharacter::ServerDropItem_Implementation(class UItem* Item, const int32 Quantity)
{
	DropItem(Item, Quantity);
}

bool ASurvivalCharacter::ServerDropItem_Validate(class UItem* Item, const int32 Quantity)
{
	return true;
}

bool ASurvivalCharacter::EquipItem(class UEquippableItem* Item)
{
	EquippedItems.Add(Item->Slot, Item);
	OnEquippedItemsChanged.Broadcast(Item->Slot, Item);
	return true;
}

bool ASurvivalCharacter::UnEquipItem(class UEquippableItem* Item)
{
	// If the item is valid
	if (Item)
	{
		// If we have the item equipped
		if (EquippedItems.Contains(Item->Slot))
		{
			// If we have the item we're unequipping
			if (Item == *EquippedItems.Find(Item->Slot))
			{
				EquippedItems.Remove(Item->Slot);
				OnEquippedItemsChanged.Broadcast(Item->Slot, nullptr);
				return true;
			}
		}
	}

	return false;
}

void ASurvivalCharacter::EquipGear(class UGearItem* Gear)
{
	// Get the slot of the gear
	if (USkeletalMeshComponent* GearMesh = *PlayerMeshes.Find(Gear->Slot))
	{
		// Set the skeletal mesh to be the gear's mesh
		GearMesh->SetSkeletalMesh(Gear->Mesh);
		GearMesh->SetMaterial(GearMesh->GetMaterials().Num() - 1, Gear->MaterialInstance);
	}

}

void ASurvivalCharacter::UnEquipGear(const EEquippableSlot Slot)
{
	// Find the skeletal mesh that we're unequipping from
	if (USkeletalMeshComponent* EquippableMesh = *PlayerMeshes.Find(Slot))
	{
		// Get the naked mesh
		if (USkeletalMesh* BodyMesh = *NakedMeshes.Find(Slot))
		{
			// Set the equippable mesh as the body mesh (naked mesh)
			EquippableMesh->SetSkeletalMesh(BodyMesh);

			// Put the materials back on the body mesh (since gear may have applied a different material)
			for (int32 i = 0; i < BodyMesh->Materials.Num(); ++i)
			{
				if (BodyMesh->Materials.IsValidIndex(i))
				{
					EquippableMesh->SetMaterial(i, BodyMesh->Materials[i].MaterialInterface);
				}
			}
		}
		else
		{
			// For some gear like backpacks, there is no naked mesh
			EquippableMesh->SetSkeletalMesh(nullptr);
		}
	}
}

void ASurvivalCharacter::EquipWeapon(class UWeaponItem* WeaponItem)
{
	if (WeaponItem && WeaponItem->WeaponClass && HasAuthority())
	{
		if (EquippedWeapon)
		{
			UnEquipWeapon();
		}

		// Spawn the weapon in
		FActorSpawnParameters SpawnParams;
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParams.Owner = SpawnParams.Instigator = this;

		if (AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponItem->WeaponClass, SpawnParams))
		{
			Weapon->Item = WeaponItem;

			EquippedWeapon = Weapon;
			OnRep_EquippedWeapon();

			Weapon->OnEquip();
		}
	}
}

void ASurvivalCharacter::UnEquipWeapon()
{
	if (HasAuthority() && EquippedWeapon)
	{
		EquippedWeapon->OnUnEquip();
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
		OnRep_EquippedWeapon();
	}
}

class USkeletalMeshComponent* ASurvivalCharacter::GetSlotSkeletalMeshComponent(const EEquippableSlot Slot)
{
	if (PlayerMeshes.Contains(Slot))
	{
		return *PlayerMeshes.Find(Slot);
	}

	return nullptr;
}

void ASurvivalCharacter::ServerUseThrowable_Implementation()
{
	UseThrowable();
}

void ASurvivalCharacter::MulticastPlayThrowableTossFX_Implementation(UAnimMontage* MontageToPlay)
{
	// Local player already instantly played grenade throw anim
	// All the other players will see us throw the item!
	if (GetNetMode() != NM_DedicatedServer && !IsLocallyControlled())
	{
		PlayAnimMontage(MontageToPlay);
	}
}

class UThrowableItem* ASurvivalCharacter::GetThrowable() const
{
	UThrowableItem* EquippedThrowable = nullptr;

	if (EquippedItems.Contains(EEquippableSlot::EIS_ThrowableItem))
	{
		EquippedThrowable = Cast<UThrowableItem>(*EquippedItems.Find(EEquippableSlot::EIS_ThrowableItem));
	}

	return EquippedThrowable;
}

void ASurvivalCharacter::UseThrowable()
{
	if (CanUseThrowable())
	{
		if (UThrowableItem* Throwable = GetThrowable())
		{
			if (HasAuthority())
			{
				SpawnThrowable();

				if (PlayerInventory)
				{
					PlayerInventory->ConsumeItem(Throwable, 1);
				}
			}
			else
			{
				if (Throwable->GetQuantity() <= 1)
				{
					EquippedItems.Remove(EEquippableSlot::EIS_ThrowableItem);
					OnEquippedItemsChanged.Broadcast(EEquippableSlot::EIS_ThrowableItem, nullptr);
				}

				// Locally play the grenade throw instantly - by the time the Server spawns the grenade in the throw animation, it should roughly sync up w/ the spawn
				PlayAnimMontage(Throwable->ThrowableTossAnimation);
				ServerUseThrowable();
			}
		}
	}
}

void ASurvivalCharacter::SpawnThrowable()
{
	if (HasAuthority())
	{
		if (UThrowableItem* CurrentThrowable = GetThrowable())
		{
			if (CurrentThrowable->ThrowableClass)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = SpawnParams.Instigator = this;
				SpawnParams.bNoFail = true;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				FVector EyesLoc;
				FRotator EyesRot;

				GetController()->GetPlayerViewPoint(EyesLoc, EyesRot);

				// Spawn throwable slightly in front of our face so it doesn't collide w/ our player
				EyesLoc = (EyesRot.Vector() * 20.f) + EyesLoc;

				if (AThrowableWeapon* ThrowableWeapon = GetWorld()->SpawnActor<AThrowableWeapon>(CurrentThrowable->ThrowableClass, FTransform(EyesRot, EyesLoc)))
				{
					MulticastPlayThrowableTossFX(CurrentThrowable->ThrowableTossAnimation);
				}
			}
		}
	}
}

bool ASurvivalCharacter::CanUseThrowable() const
{
	return GetThrowable() != nullptr && GetThrowable()->ThrowableClass != nullptr;
}

float ASurvivalCharacter::ModifyHealth(const float Delta)
{
	const float OldHealth = Health;

	Health = FMath::Clamp<float>(Health + Delta, 0.f, MaxHealth);

	return Health - OldHealth;
}

void ASurvivalCharacter::OnRep_Health(float OldHealth)
{
	OnHealthModified(Health - OldHealth);
}

void ASurvivalCharacter::OnRep_EquippedWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->OnEquip();
	}
}

void ASurvivalCharacter::StartFire()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->StartFire();
	}
	else
	{
		BeginMeleeAttack();
	}
}

void ASurvivalCharacter::StopFire()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->StopFire();
	}
}

void ASurvivalCharacter::BeginMeleeAttack()
{
	if (GetWorld()->TimeSince(LastMeleeAttackTime) > MeleeAttackMontage->GetPlayLength())
	{
		FHitResult Hit;
		FCollisionShape Shape = FCollisionShape::MakeSphere(15.f);

		FVector StartTrace = CameraComponent->GetComponentLocation();
		FVector EndTrace = (CameraComponent->GetComponentRotation().Vector() * MeleeAttackDistance) + StartTrace;

		FCollisionQueryParams QueryParams = FCollisionQueryParams("MeleeSweep", false, this);

		PlayAnimMontage(MeleeAttackMontage);

		if (GetWorld()->SweepSingleByChannel(Hit, StartTrace, EndTrace, FQuat(), COLLISION_WEAPON, Shape, QueryParams))
		{
			UE_LOG(LogTemp, Warning, TEXT("We just hit something with our punch!"));

			if (ASurvivalCharacter* HitPlayer = Cast<ASurvivalCharacter>(Hit.GetActor()))
			{
				if (ASurvivalPlayerController* PC = Cast<ASurvivalPlayerController>(GetController()))
				{
					PC->OnHitPlayer();
				}
			}
		}

		ServerProcessMeleeHit(Hit);

		LastMeleeAttackTime = GetWorld()->GetTimeSeconds();
	}
}

void ASurvivalCharacter::ServerProcessMeleeHit_Implementation(const FHitResult& MeleeHit)
{
	if (GetWorld()->TimeSince(LastMeleeAttackTime) > MeleeAttackMontage->GetPlayLength() && (GetActorLocation() - MeleeHit.ImpactPoint).Size() <= MeleeAttackDistance)
	{
		MulticastPlayMeleeFX();

		UGameplayStatics::ApplyPointDamage(MeleeHit.GetActor(), MeleeAttackDamage, (MeleeHit.TraceStart - MeleeHit.TraceEnd).GetSafeNormal(), MeleeHit, GetController(), this, UMeleeDamage::StaticClass());
	}

	LastMeleeAttackTime = GetWorld()->GetTimeSeconds();
}

void ASurvivalCharacter::MulticastPlayMeleeFX_Implementation()
{
	// If another player punches, play the animation so we can see them punch
	if (!IsLocallyControlled())
	{
		PlayAnimMontage(MeleeAttackMontage);
	}
}

void ASurvivalCharacter::OnSuicide(struct FDamageEvent const& DamageEvent, const AActor* DamageCauser)
{
	Killer = this;
	OnRep_Killer();
}

void ASurvivalCharacter::KilledByPlayer(struct FDamageEvent const& DamageEvent, class ASurvivalCharacter* Character, const AActor* DamageCauser)
{
	Killer = Character;
	OnRep_Killer();
}

void ASurvivalCharacter::OnRep_Killer()
{
	// Removing player after 20 seconds
	SetLifeSpan(20.f);

	// Turning the mesh's collision on
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	GetMesh()->SetOwnerNoSee(false);

	// Turning capsule's collision off
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	SetReplicateMovement(false);

	LootPlayerInteraction->Activate();

	// Unequip all equipped items so they can be looted by other players
	if (HasAuthority())
	{
		TArray<UEquippableItem*> Equippables;
		EquippedItems.GenerateValueArray(Equippables);

		for (auto& EquippedItem : Equippables)
		{
			EquippedItem->SetEquipped(false);
		}
	}

	if (IsLocallyControlled())
	{
		SpringArmComponent->TargetArmLength = 500.f;
		SpringArmComponent->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		bUseControllerRotationPitch = true;

		if (ASurvivalPlayerController* PC = Cast<ASurvivalPlayerController>(GetController()))
		{
			PC->ShowDeathScreen(Killer);
		}
	}
}

bool ASurvivalCharacter::CanSprint()
{
	return !IsAiming();
}

void ASurvivalCharacter::StartSprinting()
{
	SetSprinting(true);
}

void ASurvivalCharacter::StopSprinting()
{
	SetSprinting(false);
}

void ASurvivalCharacter::SetSprinting(const bool bNewSprinting)
{
	if ((bNewSprinting && !CanSprint()) || bNewSprinting == bSprinting)
	{
		return;
	}

	if (HasAuthority())
	{
		ServerSetSprinting(bNewSprinting);
	}

	bSprinting = bNewSprinting;

	GetCharacterMovement()->MaxWalkSpeed = bSprinting ? SprintSpeed : WalkSpeed;
}

void ASurvivalCharacter::ServerSetSprinting_Implementation(const bool bNewSprinting)
{
	SetSprinting(bNewSprinting);
}

bool ASurvivalCharacter::ServerSetSprinting_Validate(const bool bNewSprinting)
{
	return true;
}

void ASurvivalCharacter::StartCrouching()
{
	Crouch();
}

void ASurvivalCharacter::StopCrouching()
{
	UnCrouch();
}

void ASurvivalCharacter::MoveForward(float Val)
{
	if (Val != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Val);
	}
}

void ASurvivalCharacter::MoveRight(float Val)
{
	if (Val != 0.f)
	{
		AddMovementInput(GetActorRightVector(), Val);
	}
}

void ASurvivalCharacter::LookUp(float Val)
{
	if (Val != 0.f)
	{
		AddControllerPitchInput(Val);
	}
}

void ASurvivalCharacter::Turn(float Val)
{
	if (Val != 0.f)
	{
		AddControllerYawInput(Val);
	}
}

bool ASurvivalCharacter::CanAim() const
{
	return EquippedWeapon != nullptr;
}

void ASurvivalCharacter::StartAiming()
{
	if (CanAim())
	{
		SetAiming(true);
	}
}

void ASurvivalCharacter::StopAiming()
{
	SetAiming(false);
}

void ASurvivalCharacter::SetAiming(const bool bNewAiming)
{
	// If you can't aim or you're trying to set aiming to what it already is, just return
	if ((bNewAiming && !CanAim()) || bNewAiming == bIsAiming)
	{
		return;
	}

	if (HasAuthority())
	{
		ServerSetAiming(bNewAiming);
	}

	bIsAiming = bNewAiming;
}

void ASurvivalCharacter::ServerSetAiming_Implementation(const bool bNewAiming)
{
	SetAiming(bNewAiming);
}

// Called every frame
void ASurvivalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If the server is the authority + we're interacting, then we're gonna do the check
	//const bool bIsInteractingOnServer = (HasAuthority() && IsInteracting());

	// If the last interaction check time is greater than the check freq, we can check again
	if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}

	if (IsLocallyControlled())
	{
		const float DesiredFOV = IsAiming() ? 70.f : 100.f;
		CameraComponent->SetFieldOfView(FMath::FInterpTo(CameraComponent->FieldOfView, DesiredFOV, DeltaTime, 10.f));

		if (EquippedWeapon)
		{
			const FVector ADSLocation = EquippedWeapon->GetWeaponMesh()->GetSocketLocation(NAME_AimDownSightsSocket);
			const FVector DefaultCameraLocation = GetMesh()->GetSocketLocation(FName("Head"));

			FVector CameraLoc = bIsAiming ? ADSLocation : DefaultCameraLocation;

			const float InterpSpeed = FVector::Dist(ADSLocation, DefaultCameraLocation) / EquippedWeapon->ADSTime;
			CameraComponent->SetWorldLocation(FMath::VInterpTo(CameraComponent->GetComponentLocation(), CameraLoc, DeltaTime, InterpSpeed));
		}
	}
}

void ASurvivalCharacter::Restart()
{
	Super::Restart();

	// If the player controller is the controller
	if (ASurvivalPlayerController* PC = Cast<ASurvivalPlayerController>(GetController()))
	{
		PC->ShowIngameUI();
	}
}

float ASurvivalCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	const float DamageDealt = ModifyHealth(-Damage);

	if (Health <= 0.f)
	{
		if (ASurvivalCharacter* KillerCharacter = Cast<ASurvivalCharacter>(DamageCauser->GetOwner()))
		{
			KilledByPlayer(DamageEvent, KillerCharacter, DamageCauser);
		}
		else
		{
			OnSuicide(DamageEvent, DamageCauser);
		}
	}

	return DamageDealt;
}

void ASurvivalCharacter::SetActorHiddenInGame(bool bNewHidden)
{
	Super::SetActorHiddenInGame(bNewHidden);

	if (EquippedWeapon)
	{
		EquippedWeapon->SetActorHiddenInGame(bNewHidden);
	}
}

void ASurvivalCharacter::SetLootSource(class UInventoryComponent* NewLootSource)
{
	// If the item we're looting gets destroyed, we need to tell the client to remove their loot screen
	if (NewLootSource && NewLootSource->GetOwner())
	{
		NewLootSource->GetOwner()->OnDestroyed.AddUniqueDynamic(this, &ASurvivalCharacter::OnLootSourceOwnerDestroyed);
	}

	if (HasAuthority())
	{
		if (NewLootSource)
		{
			// Looting a player keeps their body alive for an extra 2 minutes to provide enough time to loot their items
			if (ASurvivalCharacter* Character = Cast<ASurvivalCharacter>(NewLootSource->GetOwner()))
			{
				Character->SetLifeSpan(120.f);
			}
		}

		LootSource = NewLootSource;
	}
	else
	{
		ServerSetLootSource(NewLootSource);
	}
}

bool ASurvivalCharacter::IsLooting() const
{
	return LootSource != nullptr;
}

void ASurvivalCharacter::BeginLootingPlayer(class ASurvivalCharacter* Character)
{
	if (Character)
	{
		Character->SetLootSource(PlayerInventory);
	}
}

void ASurvivalCharacter::ServerSetLootSource_Implementation(class UInventoryComponent* NewLootSource)
{
	SetLootSource(NewLootSource);
}

bool ASurvivalCharacter::ServerSetLootSource_Validate(class UInventoryComponent* NewLootSource)
{
	return true;
}

void ASurvivalCharacter::OnLootSourceOwnerDestroyed(AActor* DestroyedActor)
{
	// Remove loot source
	if (HasAuthority() && LootSource && DestroyedActor == LootSource->GetOwner())
	{
		ServerSetLootSource(nullptr);
	}
}

void ASurvivalCharacter::OnRep_LootSource()
{
	// Bring up/remove the looting menu
	if (ASurvivalPlayerController* PC = Cast<ASurvivalPlayerController>(GetController()))
	{
		if (PC->IsLocalController())
		{
			if (LootSource)
			{
				PC->ShowLootMenu(LootSource);
			}
			else
			{
				PC->HideLootMenu();
			}
		}
	}
}

void ASurvivalCharacter::LootItem(class UItem* ItemToGive)
{
	if (HasAuthority())
	{
		if (PlayerInventory && LootSource && ItemToGive && LootSource->HasItem(ItemToGive->GetClass(), ItemToGive->GetQuantity()))
		{
			const FItemAddResult AddResult = PlayerInventory->TryAddItem(ItemToGive);

			if (AddResult.ActualAmountGiven > 0)
			{
				LootSource->ConsumeItem(ItemToGive, AddResult.ActualAmountGiven);
			}
			else
			{
				// Tell the player why they couldn't loot the item
				if (ASurvivalPlayerController* PC = Cast<ASurvivalPlayerController>(GetController()))
				{
					PC->ClientShowNotification(AddResult.ErrorText);
				}
			}
		}
	}
	else
	{
		ServerLootItem(ItemToGive);
	}
}

void ASurvivalCharacter::ServerLootItem_Implementation(class UItem* ItemToLoot)
{
	LootItem(ItemToLoot);
}

bool ASurvivalCharacter::ServerLootItem_Validate(class UItem* ItemToLoot)
{
	return true;
}

void ASurvivalCharacter::PerformInteractionCheck()
{
	// If the controller is null, don't do anything
	if (GetController() == nullptr)
	{
		return;
	}

	// Keeping track of the last time we checked for an interactable obj
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector EyesLoc;
	FRotator EyesRot;

	// Returns the location + rotation of the cam
	GetController()->GetPlayerViewPoint(EyesLoc, EyesRot);

	// Where we start our ray
	FVector TraceStart = EyesLoc;

	// This is gonna give us some point where the player is looking
	FVector TraceEnd = (EyesRot.Vector() * InteractionCheckDistance) + TraceStart;
	FHitResult TraceHit;

	FCollisionQueryParams QueryParams;
	// Don't hit our character
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		// Check if the TraceHit has an Actor (that means we hit something)
		if (TraceHit.GetActor())
		{
			// Check if that thing has an interaction comp on it
			if (UInteractionComponent* InteractionComponent = Cast<UInteractionComponent>(TraceHit.GetActor()->GetComponentByClass(UInteractionComponent::StaticClass())))
			{
				// How far we're from the obj
				float Distance = (TraceStart - TraceHit.ImpactPoint).Size();

				// If the interactable we're looking at isn't equal to the interactable we were looking at last frame + the distance is in range
				if (InteractionComponent != GetInteractable() && Distance <= InteractionComponent->InteractionDistance)
				{
					// We've found a new interactable!
					FoundNewInteractable(InteractionComponent);
				}
				// Else, it's too far away
				else if (Distance > InteractionComponent->InteractionDistance && GetInteractable())
				{
					CouldntFindInteractable();
				}

				return;
			}
		}
	}

	CouldntFindInteractable();
}

void ASurvivalCharacter::CouldntFindInteractable()
{
	// Since we lost focus of the interactable, clear the timer
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_Interact))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interact);
	}
	
	// Tell the interactable we've stopped focusing on it + clear the current interactable
	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->EndFocus(this);

		// Ending the interaction w/ that obj
		if (InteractionData.bInteractHeld)
		{
			EndInteract();
		}
	}

	InteractionData.ViewedInteractionComponent = nullptr;
}

void ASurvivalCharacter::FoundNewInteractable(UInteractionComponent* Interactable)
{
	UE_LOG(LogTemp, Warning, TEXT("We found an interactable!!!"));

	// End the interact of the old interactable
	EndInteract();

	// Unfocus + remove the outline of the old interactable
	if (UInteractionComponent* OldInteractable = GetInteractable())
	{
		OldInteractable->EndFocus(this);
	}

	// Set the viewed interaction comp to the new interactable
	InteractionData.ViewedInteractionComponent = Interactable;
	// And begin focusing on it
	Interactable->BeginFocus(this);
}

void ASurvivalCharacter::BeginInteract()
{
	// If we're not the server, tell the server to BeginInteract
	if (!HasAuthority())
	{
		ServerBeginInteract();
	}

	// Optimization, the Server only checks that we're looking at an item once we begin interacting w/ it. This saves the Server doing a check every tick for an interactable Item. The exception is a non-instant interact. In this case, the server will check every tick for the duration of the interact
	if (HasAuthority())
	{
		PerformInteractionCheck();
	}

	// Keeping track if you're holding the E key down
	InteractionData.bInteractHeld = true;

	// If you have an interaction comp
	if (UInteractionComponent* Interactable = GetInteractable())
	{
		// Call BeginInteract() on the interaction comp
		Interactable->BeginInteract(this);

		// If the interaction time is basically 0, interact w/ it straight away
		if (FMath::IsNearlyZero(Interactable->InteractionTime))
		{
			Interact();
		}
		// Else, we have a timed interact so we'll queue it up + do it soon
		else
		{
			GetWorldTimerManager().SetTimer(TimerHandle_Interact, this, &ASurvivalCharacter::Interact, Interactable->InteractionTime, false);
		}
	}
}

void ASurvivalCharacter::EndInteract()
{
	// If we're not the server, tell the server to EndInteract
	if (!HasAuthority())
	{
		ServerEndInteract();
	}

	// Keeping track if you're holding the E key down
	InteractionData.bInteractHeld = false;

	// For when we let go of the E key early, clear the timer
	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	// If we have an interactable, call EndInteract() on it
	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->EndInteract(this);
	}
}

void ASurvivalCharacter::ServerBeginInteract_Implementation()
{
	// Just calling BeginInteract() on the server
	BeginInteract();
}

bool ASurvivalCharacter::ServerBeginInteract_Validate()
{
	return true;
}

void ASurvivalCharacter::ServerEndInteract_Implementation()
{
	// Just calling EndInteract() on the server
	EndInteract();
}

bool ASurvivalCharacter::ServerEndInteract_Validate()
{
	return true;
}

void ASurvivalCharacter::Interact()
{
	// If we have a duration based interaction queued, clear its timer as we're about to do it
	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	// Take the interactable + call its Interact()
	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->Interact(this);
	}
}

void ASurvivalCharacter::StartReload()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->StartReload();
	}
}

// Called to bind functionality to input
void ASurvivalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASurvivalCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASurvivalCharacter::StopFire);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ASurvivalCharacter::StartAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ASurvivalCharacter::StopAiming);
	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &ASurvivalCharacter::UseThrowable);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ASurvivalCharacter::BeginInteract);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ASurvivalCharacter::EndInteract);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASurvivalCharacter::StartCrouching);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASurvivalCharacter::StopCrouching);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASurvivalCharacter::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASurvivalCharacter::StopSprinting);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASurvivalCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASurvivalCharacter::MoveRight);
}

#undef LOCTEXT_NAMESPACE