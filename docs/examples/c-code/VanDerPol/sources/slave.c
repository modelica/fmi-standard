/****************************************************************
 *  Copyright (c) Dassault Systemes. All rights reserved.       *
 *  This file is part of the Test-FMUs. See LICENSE.txt in the  *
 *  project root for license information.                       *
 ****************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "config.h"
#include "slave.h"

ModelInstance *createModelInstance(
	loggerType cbLogger,
	allocateMemoryType cbAllocateMemory,
	freeMemoryType cbFreeMemory,
	void *componentEnvironment,
	const char *instanceName,
	const char *GUID,
	const char *resourceLocation,
	bool loggingOn,
	InterfaceType interfaceType) {

	ModelInstance *comp = NULL;

	if (!cbLogger) {
		return NULL;
	}

	if (!cbAllocateMemory || !cbFreeMemory) {
		cbLogger(componentEnvironment, instanceName, Error, "error", "Missing callback function.");
		return NULL;
	}

	if (!instanceName || strlen(instanceName) == 0) {
		cbLogger(componentEnvironment, "?", Error, "error", "Missing instance name.");
		return NULL;
	}

	if (!GUID || strlen(GUID) == 0) {
		cbLogger(componentEnvironment, instanceName, Error, "error", "Missing GUID.");
		return NULL;
	}

	if (strcmp(GUID, MODEL_GUID)) {
		cbLogger(componentEnvironment, instanceName, Error, "error", "Wrong GUID.");
		return NULL;
	}

#if FMI_VERSION < 3
	comp = (ModelInstance *)cbAllocateMemory(1, sizeof(ModelInstance));
#else
	comp = (ModelInstance *)cbAllocateMemory(componentEnvironment, 1, sizeof(ModelInstance));
#endif

	if (comp) {

		// set the callbacks
		comp->componentEnvironment = componentEnvironment;
		comp->logger = cbLogger;
		comp->allocateMemory = cbAllocateMemory;
		comp->freeMemory = cbFreeMemory;

		comp->instanceName = (char *)allocateMemory(comp, 1 + strlen(instanceName), sizeof(char));

		// resourceLocation is NULL for FMI 1.0 ME
		if (resourceLocation) {
			comp->resourceLocation = (char *)allocateMemory(comp, 1 + strlen(resourceLocation), sizeof(char));
			strcpy((char *)comp->resourceLocation, (char *)resourceLocation);
		} else {
			comp->resourceLocation = NULL;
		}

		comp->modelData = (ModelData *)allocateMemory(comp, 1, sizeof(ModelData));
        
        comp->logEvents = loggingOn;
        comp->logErrors = true; // always log errors
	}

	if (!comp || !comp->modelData || !comp->instanceName) {
		logError(comp, "Out of memory.");
		return NULL;
	}

	comp->time = 0; // overwrite in fmi*SetupExperiment, fmi*SetTime
	strcpy((char *)comp->instanceName, (char *)instanceName);

	comp->state = modelInstantiated;
	comp->isNewEventIteration = false;

	comp->newDiscreteStatesNeeded = false;
	comp->terminateSimulation = false;
	comp->nominalsOfContinuousStatesChanged = false;
	comp->valuesOfContinuousStatesChanged = false;
	comp->nextEventTimeDefined = false;
	comp->nextEventTime = 0;

	setStartValues(comp); // to be implemented by the includer of this file
	comp->isDirtyValues = true; // because we just called setStartValues

#if NUMBER_OF_EVENT_INDICATORS > 0
	comp->z = allocateMemory(comp, sizeof(double), NUMBER_OF_EVENT_INDICATORS);
	comp->prez = allocateMemory(comp, sizeof(double), NUMBER_OF_EVENT_INDICATORS);
#else
	comp->z = NULL;
	comp->prez = NULL;
#endif

	return comp;
}

void freeModelInstance(ModelInstance *comp) {
	freeMemory(comp, (void *)comp->instanceName);
	freeMemory(comp, (void *)comp->z);
	freeMemory(comp, (void *)comp->prez);
	freeMemory(comp, comp);
}

void *allocateMemory(ModelInstance *comp, size_t num, size_t size) {
#if FMI_VERSION > 2
	return comp->allocateMemory(comp->componentEnvironment, num, size);
#else
	return comp->allocateMemory(num, size);
#endif
}

void freeMemory(ModelInstance *comp, void *obj) {
#if FMI_VERSION > 2
	comp->freeMemory(comp->componentEnvironment, obj);
#else
	comp->freeMemory(obj);
#endif
}

const char *duplicateString(ModelInstance *comp, const char *str1) {
	size_t len = strlen(str1);
	char *str2 = allocateMemory(comp, len + 1, sizeof(char));
	strncpy(str2, str1, len + 1);
	return str2;
}

bool invalidNumber(ModelInstance *comp, const char *f, const char *arg, size_t actual, size_t expected) {
	
	if (actual != expected) {
		comp->state = modelError;
		logError(comp, "%s: Invalid argument %s = %d. Expected %d.", f, arg, actual, expected);
		return true;
	}
	
	return false;
}

bool invalidState(ModelInstance *comp, const char *f, int statesExpected) {
	
	if (!comp) {
		return true;
	}

	if (!(comp->state & statesExpected)) {
		comp->state = modelError;
		logError(comp, "%s: Illegal call sequence.", f);
		return true;
	}
	
	return false;
}

bool nullPointer(ModelInstance* comp, const char *f, const char *arg, const void *p) {

	if (!p) {
		comp->state = modelError;
		logError(comp, "%s: Invalid argument %s = NULL.", f, arg);
		return true;
	}
	
	return false;
}

Status setDebugLogging(ModelInstance *comp, bool loggingOn, size_t nCategories, const char * const categories[]) {
    
    if (loggingOn) {
        for (size_t i = 0; i < nCategories; i++) {
            if (categories[i] == NULL) {
                logError(comp, "Log category[%d] must not be NULL", i);
                return Error;
            } else if (strcmp(categories[i], "logEvents") == 0) {
                comp->logEvents = true;
            } else if (strcmp(categories[i], "logStatusError") == 0) {
                comp->logErrors = true;
            } else {
                logError(comp, "Log category[%d] must be one of logEvents or logStatusError but was %s", i, categories[i]);
                return Error;
            }
        }
    } else {
        // disable logging
        comp->logEvents = false;
        comp->logErrors = false;
    }
    
    return OK;
}

static void logMessage(ModelInstance *comp, int status, const char *category, const char *message, va_list args) {
    
    va_list args1;
    size_t len = 0;
    char *buf = "";
    
    va_copy(args1, args);
    len = vsnprintf(buf, len, message, args1);
    va_end(args1);
    
    va_copy(args1, args);
    buf = allocateMemory(comp, len + 1, sizeof(char));
    vsnprintf(buf, len + 1, message, args);
    va_end(args1);
    
    // no need to distinguish between FMI versions since we're not using variadic arguments
    comp->logger(comp->componentEnvironment, comp->instanceName, status, category, buf);
    
    freeMemory(comp, buf);
}

void logEvent(ModelInstance *comp, const char *message, ...) {
    
    if (!comp->logEvents) return;
    
    va_list args;
    va_start(args, message);
    logMessage(comp, OK, "logEvents", message, args);
    va_end(args);
}

void logError(ModelInstance *comp, const char *message, ...) {
    
    if (!comp->logErrors) return;

    va_list args;
    va_start(args, message);
    logMessage(comp, Error, "logStatusError", message, args);
    va_end(args);
}

// default implementations
#if NUMBER_OF_EVENT_INDICATORS < 1
void getEventIndicators(ModelInstance *comp, double z[], size_t nz) {
	UNUSED(comp)
	UNUSED(z)
	UNUSED(nz)
    // do nothing
}
#endif

#ifndef GET_FLOAT64
Status getFloat64(ModelInstance* comp, ValueReference vr, double *value, size_t *index) {
	UNUSED(comp)
	UNUSED(vr)
	UNUSED(value)
	UNUSED(index)
    return Error;
}
#endif

#ifndef GET_INT32
Status getInt32(ModelInstance* comp, ValueReference vr, int *value, size_t *index) {
	UNUSED(comp)
	UNUSED(vr)
	UNUSED(value)
	UNUSED(index)
	return Error;
}
#endif

#ifndef GET_BOOLEAN
Status getBoolean(ModelInstance* comp, ValueReference vr, bool *value, size_t *index) {
	UNUSED(comp)
	UNUSED(vr)
	UNUSED(value)
	UNUSED(index)
    return Error;
}
#endif

#ifndef GET_STRING
Status getString(ModelInstance* comp, ValueReference vr, const char **value, size_t *index) {
	UNUSED(comp)
	UNUSED(vr)
	UNUSED(value)
	UNUSED(index)
    return Error;
}
#endif

#ifndef GET_BINARY
Status getBinary(ModelInstance* comp, ValueReference vr, size_t size[], const char *value[], size_t *index) {
	UNUSED(comp)
	UNUSED(vr)
	UNUSED(size)
	UNUSED(value)
	UNUSED(index)
    return Error;
}
#endif

#ifndef SET_FLOAT64
Status setFloat64(ModelInstance* comp, ValueReference vr, const double *value, size_t *index) {
	UNUSED(comp)
	UNUSED(vr)
	UNUSED(value)
	UNUSED(index)
    return Error;
}
#endif

#ifndef SET_INT32
Status setInt32(ModelInstance* comp, ValueReference vr, const int *value, size_t *index) {
	UNUSED(comp)
	UNUSED(vr)
	UNUSED(value)
	UNUSED(index)
    return Error;
}
#endif

#ifndef SET_BOOLEAN
Status setBoolean(ModelInstance* comp, ValueReference vr, const bool *value, size_t *index) {
	UNUSED(comp)
	UNUSED(vr)
	UNUSED(value)
	UNUSED(index)
    return Error;
}
#endif

#ifndef SET_STRING
Status setString(ModelInstance* comp, ValueReference vr, const char *const *value, size_t *index) {
	UNUSED(comp)
	UNUSED(vr)
	UNUSED(value)
	UNUSED(index)
    return Error;
}
#endif

#ifndef SET_BINARY
Status setBinary(ModelInstance* comp, ValueReference vr, const size_t size[], const char *const value[], size_t *index) {
	UNUSED(comp)
	UNUSED(vr)
	UNUSED(size)
	UNUSED(value)
	UNUSED(index)
    return Error;
}
#endif

#if NUMBER_OF_STATES < 1
void getContinuousStates(ModelInstance *comp, double x[], size_t nx) {
	UNUSED(comp)
	UNUSED(x)
	UNUSED(nx)
}

void setContinuousStates(ModelInstance *comp, const double x[], size_t nx) {
	UNUSED(comp)
	UNUSED(x)
	UNUSED(nx)
}

void getDerivatives(ModelInstance *comp, double dx[], size_t nx) {
	UNUSED(comp)
	UNUSED(dx)
	UNUSED(nx)
}
#endif

#ifndef GET_PARTIAL_DERIVATIVE
Status getPartialDerivative(ModelInstance *comp, ValueReference unknown, ValueReference known, double *partialDerivative) {
	UNUSED(comp)
	UNUSED(unknown)
	UNUSED(known)
	UNUSED(partialDerivative)
	return Error;
}
#endif

Status doStep(ModelInstance *comp, double t, double tNext) {

	UNUSED(t)

    bool stateEvent, timeEvent;
    
#if NUMBER_OF_EVENT_INDICATORS > 0
	double *temp = NULL;
#endif
    
#if NUMBER_OF_STATES > 0
	double  x[NUMBER_OF_STATES] = { 0 };
	double dx[NUMBER_OF_STATES] = { 0 };
#endif

    while (comp->time + FIXED_SOLVER_STEP < tNext + 0.1 * FIXED_SOLVER_STEP) {
        
#if NUMBER_OF_STATES > 0
		getContinuousStates(comp, x, NUMBER_OF_STATES);
		getDerivatives(comp, dx, NUMBER_OF_STATES);

		// forward Euler step
		for (int i = 0; i < NUMBER_OF_STATES; i++) {
			x[i] += FIXED_SOLVER_STEP * dx[i];
		}

		setContinuousStates(comp, x, NUMBER_OF_STATES);
#endif

        stateEvent = false;

#if NUMBER_OF_EVENT_INDICATORS > 0
		getEventIndicators(comp, comp->z, NUMBER_OF_EVENT_INDICATORS);
		
        // check for zero-crossing
		for (int i = 0; i < NUMBER_OF_EVENT_INDICATORS; i++) {
		    stateEvent |= (comp->prez[i] * comp->z[i]) <= 0;
		}
		
		// remember the current event indicators
		temp = comp->z;
		comp->z = comp->prez;
		comp->prez = temp;
#endif
        
        // check for time event
        timeEvent = comp->nextEventTimeDefined && comp->time >= comp->nextEventTime;
        
        // log events
        if (timeEvent) logEvent(comp, "Time event detected at t=%g s.", comp->time);
        if (stateEvent) logEvent(comp, "State event detected at t=%g s.", comp->time);

        if (stateEvent || timeEvent) {
            eventUpdate(comp);
        }

        // terminate simulation, if requested by the model in the previous step
        if (comp->terminateSimulation) {
#if FMI_VERSION > 1
            comp->state = modelStepFailed;
#endif
            return Discard; // enforce termination of the simulation loop
        }

		comp->time += FIXED_SOLVER_STEP;
    }

    return OK;
}
