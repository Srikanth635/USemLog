// Copyright 2017-2020, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Individuals/SLBaseConstraintIndividual.h"
#include "SLBoneConstraintIndividual.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = SL)
class USEMLOG_API USLBoneConstraintIndividual : public USLBaseConstraintIndividual
{
	GENERATED_BODY()

public:
    // Ctor
    USLBoneConstraintIndividual();

    // Called before destroying the object.
    virtual void BeginDestroy() override;

    // Set the parameters required when initalizing the individual
    bool PreInit(int32 NewConstraintIndex, bool bReset);

    // Check if the individual is pre initalized
    bool IsPreInit() const { return bIsPreInit; };

    // Init asset references (bForced forces re-initialization)
    virtual bool Init(bool bReset);

    // Load semantic data (bForced forces re-loading)
    virtual bool Load(bool bReset, bool bTryImport);

    // Get the type name as string
    virtual FString GetTypeName() const override { return FString("BoneConstraintIndividual"); };

protected:
    // Get class name, virtual since each invidiual type will have different name
    virtual FString CalcDefaultClassValue() override;

private:
    // Set dependencies
    bool InitImpl();

    // Set data
    bool LoadImpl(bool bTryImport);

    // Clear all values of the individual
    void InitReset();

    // Clear all data of the individual
    void LoadReset();

protected:
    // Needs to be set in order to be initialized
    UPROPERTY(VisibleAnywhere, Category = "SL")
    bool bIsPreInit;

    // Position of this constraint within the array in the SkeletalMeshComponent
    UPROPERTY(VisibleAnywhere, Category = "SL")
    int32 ConstraintIndex;
};
