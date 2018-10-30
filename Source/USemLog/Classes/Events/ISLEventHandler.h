// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

#pragma once

#include "Events/ISLEvent.h"

/** Delegate for notification of finished semantic events */
DECLARE_DELEGATE_OneParam(FSLEventSignature, TSharedPtr<ISLEvent>);


/**
 * Listens to events input, and outputs finished semantic events
 */
class ISLEventHandler 
{
public:
	// Default constructor 
	ISLEventHandler() : bIsInit(false), bIsStarted(false), bIsFinished(false) {};

	// Virtual destructor
	virtual ~ISLEventHandler() {};
	
	// Init
	virtual void Init(UObject* Parent) = 0;

	// Start listening
	virtual void Start() = 0;
	
	// Terminate handler, finish and publish remaining events
	virtual void Finish(float EndTime) = 0;

	// Get init state
	bool IsInit() const { return bIsInit; };

	// Get started state
	bool IsStarted() const { return bIsStarted; };

	// Get finished state
	bool IsFinished() const { return bIsFinished; };

public:
	// Called when a semantic event is finished
	FSLEventSignature OnSemanticEvent;

protected:
	// Set when initialized
	bool bIsInit;

	// Set when started
	bool bIsStarted;

	// Set when finished
	bool bIsFinished;
};
