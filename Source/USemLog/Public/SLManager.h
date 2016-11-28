// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/Actor.h"
#include "Private/SLRawDataExporter.h"
#include "Private/SLMapExporter.h"
#include "Private/SLEventsExporter.h"
#include "SLManager.generated.h"

UCLASS()
class USEMLOG_API ASLManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASLManager();
	
	// Actor initialization, log items init
	virtual void PreInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called when the game is terminated
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Init exporters, write initial states
	void Init();

	// Start logging by enabling tick
	void Start();
	
	// Pause logging by disabling tick
	void Pause();

	// Get semantic events exporter
	class FSLEventsExporter* GetEventsExporter() { return SemEventsExporter; };

private:
	// Create directory path for logging
	void CreateDirectoryPath(FString Path);

	// Set items to be loggeed (from tags)
	void InitLogItems();

	// Check if new unique names should be generated (previous ones are not stored)
	bool ShouldGenerateNewUniqueNames(const FString Path);

	// Set unique names of items
	void GenerateNewUniqueNames();

	// Write generated unique names to file
	void StoreNewUniqueNames(const FString Path);

	// Cancel logging (e.g if objects are out of sync)
	void CancelLogging();

	// Log semantic events
	UPROPERTY(EditAnywhere, Category = "Semantic Logger")
	bool bStartLoggingAtLoadTime;

	// Directory to save the logs
	UPROPERTY(EditAnywhere, Category = "Semantic Logger")
	FString LogRootDirectoryName;

	// Log raw data
	UPROPERTY(EditAnywhere, Category = "Semantic Logger")
	bool bLogRawData;

	// Distance threshold for raw data logging (cm)
	UPROPERTY(EditAnywhere, Category = "Semantic Logger")
	float DistanceThreshold;

	// Log semantic map (if not already logged)
	UPROPERTY(EditAnywhere, Category = "Semantic Logger")
	bool bLogSemanticMap;

	// Log semantic events
	UPROPERTY(EditAnywhere, Category = "Semantic Logger")
	bool bLogSemanticEvents;

	// Log semantic events
	UPROPERTY(EditAnywhere, Category = "Semantic Logger")
	TArray<FString> LogFoliageClasses;

	// Map of actors to be logged to the semlog key-value pair info
	TMap<AActor*, TArray<TPair<FString, FString>>> ActorToSemLogInfo;

	// Map of actors to be logged to their unique name
	TMap<AActor*, FString> ActorToUniqueName;

	// Map of the foliage class name to the component
	TMap<FString, UInstancedStaticMeshComponent*> FoliageClassNameToComponent;

	// Map of foliage class to array of bodies with unique names 
	TMap<UInstancedStaticMeshComponent*, TArray<TPair<FBodyInstance*, FString>>> FoliageComponentToUniqueNameArray;

	// Raw data exporter
	class FSLRawDataExporter* RawDataExporter;

	// Semantic map exporter
	class FSLMapExporter* SemMapExporter;

	// Semantic events exporter
	class FSLEventsExporter* SemEventsExporter;

	// Level path
	FString LevelPath;

	// Episode path
	FString EpisodePath;

	// Raw data path
	FString RawDataPath;

	// Episode unique tag
	FString EpisodeUniqueTag;
};