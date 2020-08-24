// Copyright 2017-2020, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#include "Individuals/Type/SLBoneIndividual.h"
#include "Individuals/Type/SLSkeletalIndividual.h"
#include "Individuals/Type/SLVirtualBoneIndividual.h"
#include "Skeletal/SLSkeletalDataComponent.h"

#include "Animation/SkeletalMeshActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// Ctor
USLBoneIndividual::USLBoneIndividual()
{
	bIsPreInit = false;
	MaterialIndex = INDEX_NONE;
	BoneIndex = INDEX_NONE;
	ParentIndividual = nullptr;
}

// Called before destroying the object.
void USLBoneIndividual::BeginDestroy()
{
	SetIsInit(false);
	Super::BeginDestroy();
}

// Set the parameters required when initalizing the individual
bool USLBoneIndividual::PreInit(int32 NewBoneIndex, int32 NewMaterialIndex, bool bReset)
{
	if (bReset)
	{
		bIsPreInit = false;
	}

	if (IsPreInit())
	{
		return true;
	}

	BoneIndex = NewBoneIndex;
	MaterialIndex = NewMaterialIndex;
	TagType += "Bone" + FString::FromInt(BoneIndex);
	bIsPreInit = true;
	return true;
}

// Set pointer to the semantic owner
bool USLBoneIndividual::Init(bool bReset)
{
	if (bReset)
	{
		InitReset();
	}

	if (IsInit())
	{
		return true;
	}

	SetIsInit(Super::Init(bReset) && InitImpl());
	return IsInit();
}

// Load semantic data
bool USLBoneIndividual::Load(bool bReset, bool bTryImport)
{
	if (bReset)
	{
		LoadReset();
	}

	if (IsLoaded())
	{
		return true;
	}

	if (!IsInit())
	{
		if (!Init(bReset))
		{
			UE_LOG(LogTemp, Log, TEXT("%s::%d Cannot load  individual %s, init fails.."),
				*FString(__FUNCTION__), __LINE__, *GetFullName());
			return false;
		}
	}

	SetIsLoaded(Super::Load(bReset, bTryImport) && LoadImpl(bTryImport));
	return IsLoaded();
}

// Apply visual mask material
bool USLBoneIndividual::ApplyMaskMaterials(bool bIncludeChildren /*= false*/)
{
	if (!IsInit())
	{
		return false;
	}

	if (!bIsMaskMaterialOn)
	{
		SkeletalMeshComponent->SetMaterial(MaterialIndex, VisualMaskDynamicMaterial);
		bIsMaskMaterialOn = true;
		return true;
	}
	return false;
}

// Apply original materials
bool USLBoneIndividual::ApplyOriginalMaterials()
{
	if (!IsInit())
	{
		return false;
	}

	// Bones share the same original materials with the skeletal parent, this will only set the flag value
	if (bIsMaskMaterialOn)
	{
		// Applied in parent
		bIsMaskMaterialOn = false;
		return true;
	}
	return false;
}

// Cache the current transform of the individual (returns true on a new value)
bool USLBoneIndividual::CalcAndCacheTransform(float Tolerance, FTransform* OutTransform)
{
	if (IsInit())
	{
		const FTransform CurrTrans = SkeletalMeshComponent->GetBoneTransform(BoneIndex);
		if (!CachedTransform.Equals(CurrTrans, Tolerance))
		{
			CachedTransform = CurrTrans;
			if (OutTransform != nullptr)
			{
				*OutTransform = CachedTransform;
			}
			return true;
		}
		else
		{
			if (OutTransform != nullptr)
			{
				*OutTransform = CachedTransform;
			}
			return false;
		}
	}
	else
	{
		if (!CachedTransform.Equals(FTransform::Identity, Tolerance))
		{
			CachedTransform = FTransform::Identity;
			if (OutTransform != nullptr)
			{
				*OutTransform = CachedTransform;
			}
			return true;
		}
		else
		{
			if (OutTransform != nullptr)
			{
				*OutTransform = CachedTransform;
			}
			return false;
		}
	}
}

// Get the attachment location name (bone/socket)
FName USLBoneIndividual::GetAttachmentLocationName()
{
	if (HasValidSkeletalMeshComponent() || SetSkeletalMeshComponent())
	{
		if (HasValidBoneIndex())
		{
			return SkeletalMeshComponent->GetBoneName(BoneIndex);
		}
	}
	return NAME_None;
}

// Get class name, virtual since each invidiual type will have different name
FString USLBoneIndividual::CalcDefaultClassValue()
{
	if (USLSkeletalIndividual* SkI = Cast<USLSkeletalIndividual>(GetOuter()))
	{
		if (SkI->HasValidSkeletalDataAsset() || SkI->SetSkeletalDataAsset())
		{
			if (FString* BoneClassValue = SkI->SkeletalDataAsset->BoneIndexClass.Find(BoneIndex))
			{
				if (!BoneClassValue->IsEmpty())
				{
					return *BoneClassValue;
				}
			}
		}
	}
	return GetTypeName();
}

// Set the skeletal actor as parent
bool USLBoneIndividual::SetParentActor()
{
	if (USLSkeletalIndividual* Individual = Cast<USLSkeletalIndividual>(GetOuter()))
	{
		if (UActorComponent* AC = Cast<UActorComponent>(Individual->GetOuter()))
		{
			if (AActor* CompOwner = Cast<AActor>(AC->GetOuter()))
			{
				if (CompOwner->IsA(ASkeletalMeshActor::StaticClass()))
				{
					ParentActor = CompOwner;
					return true;
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("%s::%d %s's ParentActor should be a skeletal mesh actor.."),
						*FString(__FUNCTION__), __LINE__, *GetFullName());
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("%s::%d %s's third outer should be the parent actor.."),
					*FString(__FUNCTION__), __LINE__, *GetFullName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s::%d %s's second outer should be an actor component.."),
				*FString(__FUNCTION__), __LINE__, *GetFullName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s::%d %s's outer should be a skeletal individual object.."),
			*FString(__FUNCTION__), __LINE__, *GetFullName());
	}
	return false;
}

// Private init implementation
bool USLBoneIndividual::InitImpl()
{
	if (!IsPreInit())
	{
		UE_LOG(LogTemp, Log, TEXT("%s::%d Cannot init individual %s, pre init need to be called right after creation.."),
			*FString(__FUNCTION__), __LINE__, *GetFullName());
		return false;
	}

	// Make sure the visual mesh is set
	if (HasValidSkeletalMeshComponent() || SetSkeletalMeshComponent())
	{
		if (HasValidMaterialIndex())
		{
			SetParentIndividual();
			SetChildrenIndividuals();
			return true;
		}
	}
	return false;
}

// Private load implementation
bool USLBoneIndividual::LoadImpl(bool bTryImport)
{
	return true;
}

// Clear all values of the individual
void USLBoneIndividual::InitReset()
{
	LoadReset();
	ParentIndividual = nullptr;
	ClearChildrenIndividuals();
	SetIsInit(false);
}

// Clear all data of the individual
void USLBoneIndividual::LoadReset()
{
	SetIsLoaded(false);
}

// Check if the bone index is valid
bool USLBoneIndividual::HasValidBoneIndex() const
{
	return HasValidSkeletalMeshComponent()
		&& BoneIndex != INDEX_NONE
		&& BoneIndex < SkeletalMeshComponent->GetNumBones();
}

// Check if the material index is valid
bool USLBoneIndividual::HasValidMaterialIndex() const
{
	return HasValidSkeletalMeshComponent()
		&& MaterialIndex != INDEX_NONE
		&& MaterialIndex < SkeletalMeshComponent->GetNumMaterials();
}

// Check if the static mesh component is set
bool USLBoneIndividual::HasValidSkeletalMeshComponent() const
{
	return SkeletalMeshComponent && SkeletalMeshComponent->IsValidLowLevel() && !SkeletalMeshComponent->IsPendingKill();
}

// Set sekeletal mesh
bool USLBoneIndividual::SetSkeletalMeshComponent()
{
	if (HasValidParentActor() || SetParentActor())
	{
		if (ASkeletalMeshActor* SMA = Cast<ASkeletalMeshActor>(ParentActor))
		{
			if(USkeletalMeshComponent* SMC = SMA->GetSkeletalMeshComponent())
			{
				SkeletalMeshComponent = SMC;
				return HasValidSkeletalMeshComponent();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("%s::%d %s's ParentActor has no SkeletalMeshComponent, this should not happen.."),
					*FString(__FUNCTION__), __LINE__, *GetFullName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s::%d %s's ParentActor should be a skeletal mesh actor.."),
				*FString(__FUNCTION__), __LINE__, *GetFullName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s::%d %s ParentActor is not set, this should not happen.."),
			*FString(__FUNCTION__), __LINE__, *GetFullName());
	}
	return false;
}

// Check if a parent individual is set
bool USLBoneIndividual::HasValidParentIndividual() const
{
	return ParentIndividual && ParentIndividual->IsValidLowLevel() && !ParentIndividual->IsPendingKill();
}

// Set parent individual (if any) it might be root bone
bool USLBoneIndividual::SetParentIndividual()
{
	if (HasValidParentIndividual())
	{
		return true;
	}

	if (HasValidSkeletalMeshComponent() || SetSkeletalMeshComponent())
	{
		if (HasValidBoneIndex())
		{
			int32 ParentIndex = SkeletalMeshComponent->SkeletalMesh->RefSkeleton.GetParentIndex(BoneIndex);		
			if (ParentIndex != INDEX_NONE)
			{
				if (USLSkeletalIndividual* SkI = Cast<USLSkeletalIndividual>(GetOuter()))
				{
					if (USLBaseIndividual* BI = SkI->GetBoneIndividual(ParentIndex))
					{
						ParentIndividual = BI;
						return HasValidParentIndividual();
					}
				}
			}
		}
	}
	return false;
}

// Check if a child individual is set
bool USLBoneIndividual::HasValidChildrenIndividuals() const
{
	if (ChildrenIndividuals.Num() == 0)
	{
		return false;
	}

	for (const auto& CI : ChildrenIndividuals)
	{
		if (!CI || !CI->IsValidLowLevel() || CI->IsPendingKill())
		{
			return false;
		}
	}
	return true;
}

// Set child individual (if any) it might be a leaf bone
bool USLBoneIndividual::SetChildrenIndividuals()
{
	if (HasValidChildrenIndividuals())
	{
		return true;
	}
	// Clear any dangling children
	ClearChildrenIndividuals();

	if (HasValidSkeletalMeshComponent() || SetSkeletalMeshComponent())
	{
		if (HasValidBoneIndex())
		{
			TArray<int32> ChildrenIndexes;
			//SkeletalMeshComponent->SkeletalMesh->RefSkeleton.GetDirectChildBones(BoneIndex, ChildrenIndexes);
			SkeletalMeshComponent->SkeletalMesh->Skeleton->GetChildBones(BoneIndex, ChildrenIndexes);
			if (USLSkeletalIndividual* SkI = Cast<USLSkeletalIndividual>(GetOuter()))
			{
				for (auto ChildIndex : ChildrenIndexes)
				{
					if (USLBaseIndividual* BI = SkI->GetBoneIndividual(ChildIndex))
					{
						ChildrenIndividuals.Add(BI);
					}
				}
				return HasValidChildrenIndividuals();
			}
		}
	}
	return false;
}

// Clear children individual
void USLBoneIndividual::ClearChildrenIndividuals()
{
	ChildrenIndividuals.Empty();
}
