// Copyright 2017-2020, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/SLVisualIndividual.h"
#include "SLSkeletalIndividual.generated.h"


/**
 *
 */
USTRUCT()
struct FSLBoneIndividual
{
    GENERATED_BODY()

    // Skeletal body part individual unique id
    UPROPERTY(EditAnywhere, Category = "SL")
    FString VisualMask;

    // Skeletal body part individual class
    UPROPERTY(EditAnywhere, Category = "SL")
    FString CalibratedVisualMask;    
};

/**
 * 
 */
UCLASS()
class USEMLOG_API USLSkeletalIndividual : public USLVisualIndividual
{
	GENERATED_BODY()

protected:
    // Skeletal body part individual unique id
    UPROPERTY(EditAnywhere, Category = "SL")
    TMap<FName, FSLBoneIndividual> Bones;
};