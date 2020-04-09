/****************************************************************
 *  Copyright (c) Dassault Systemes. All rights reserved.       *
 *  This file is part of the Test-FMUs. See LICENSE.txt in the  *
 *  project root for license information.                       *
 ****************************************************************/

#if FMI_VERSION != 3
#error FMI_VERSION must be 3
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "config.h"
#include "model.h"
#include "slave.h"


// C-code FMUs have functions names prefixed with MODEL_IDENTIFIER_.
// Define DISABLE_PREFIX to build a binary FMU.
#ifndef DISABLE_PREFIX
#define pasteA(a,b)     a ## b
#define pasteB(a,b)    pasteA(a,b)
#define fmi3_FUNCTION_PREFIX pasteB(MODEL_IDENTIFIER, _)
#endif
#include "fmi3Functions.h"

#ifndef max
#define max(a,b) ((a)>(b) ? (a) : (b))
#endif

#ifndef DT_EVENT_DETECT
#define DT_EVENT_DETECT 1e-10
#endif

// ---------------------------------------------------------------------------
// Function calls allowed state masks for both Model-exchange and Co-simulation
// ---------------------------------------------------------------------------
#define MASK_fmi3GetTypesPlatform        (modelStartAndEnd | modelInstantiated | modelInitializationMode \
| modelEventMode | modelContinuousTimeMode \
| modelStepComplete | modelStepInProgress | modelStepFailed | modelStepCanceled \
| modelTerminated | modelError)
#define MASK_fmi3GetVersion              MASK_fmi3GetTypesPlatform
#define MASK_fmi3SetDebugLogging         (modelInstantiated | modelInitializationMode \
| modelEventMode | modelContinuousTimeMode \
| modelStepComplete | modelStepInProgress | modelStepFailed | modelStepCanceled \
| modelTerminated | modelError)
#define MASK_fmi3Instantiate             (modelStartAndEnd)
#define MASK_fmi3FreeInstance            (modelInstantiated | modelInitializationMode \
| modelEventMode | modelContinuousTimeMode \
| modelStepComplete | modelStepFailed | modelStepCanceled \
| modelTerminated | modelError)
#define MASK_fmi3SetupExperiment         modelInstantiated
#define MASK_fmi3EnterInitializationMode modelInstantiated
#define MASK_fmi3ExitInitializationMode  modelInitializationMode
#define MASK_fmi3Terminate               (modelEventMode | modelContinuousTimeMode \
| modelStepComplete | modelStepFailed)
#define MASK_fmi3Reset                   MASK_fmi3FreeInstance
#define MASK_fmi3GetReal                 (modelInitializationMode \
| modelEventMode | modelContinuousTimeMode \
| modelStepComplete | modelStepFailed | modelStepCanceled \
| modelTerminated | modelError)
#define MASK_fmi3GetInteger              MASK_fmi3GetReal
#define MASK_fmi3GetBoolean              MASK_fmi3GetReal
#define MASK_fmi3GetString               MASK_fmi3GetReal
#define MASK_fmi3SetReal                 (modelInstantiated | modelInitializationMode \
| modelEventMode | modelContinuousTimeMode \
| modelStepComplete)
#define MASK_fmi3SetInteger              (modelInstantiated | modelInitializationMode \
| modelEventMode \
| modelStepComplete)
#define MASK_fmi3SetBoolean              MASK_fmi3SetInteger
#define MASK_fmi3SetString               MASK_fmi3SetInteger
#define MASK_fmi3GetFMUState             MASK_fmi3FreeInstance
#define MASK_fmi3SetFMUState             MASK_fmi3FreeInstance
#define MASK_fmi3FreeFMUState            MASK_fmi3FreeInstance
#define MASK_fmi3SerializedFMUStateSize  MASK_fmi3FreeInstance
#define MASK_fmi3SerializeFMUState       MASK_fmi3FreeInstance
#define MASK_fmi3DeSerializeFMUState     MASK_fmi3FreeInstance
#define MASK_fmi3GetDirectionalDerivative (modelInitializationMode \
| modelEventMode | modelContinuousTimeMode \
| modelStepComplete | modelStepFailed | modelStepCanceled \
| modelTerminated | modelError)

// ---------------------------------------------------------------------------
// Function calls allowed state masks for Model-exchange
// ---------------------------------------------------------------------------
#define MASK_fmi3EnterEventMode          (modelEventMode | modelContinuousTimeMode)
#define MASK_fmi3NewDiscreteStates       modelEventMode
#define MASK_fmi3EnterContinuousTimeMode modelEventMode
#define MASK_fmi3CompletedIntegratorStep modelContinuousTimeMode
#define MASK_fmi3SetTime                 (modelEventMode | modelContinuousTimeMode)
#define MASK_fmi3SetContinuousStates     modelContinuousTimeMode
#define MASK_fmi3GetEventIndicators      (modelInitializationMode \
| modelEventMode | modelContinuousTimeMode \
| modelTerminated | modelError)
#define MASK_fmi3GetContinuousStates     MASK_fmi3GetEventIndicators
#define MASK_fmi3GetDerivatives          (modelEventMode | modelContinuousTimeMode \
| modelTerminated | modelError)
#define MASK_fmi3GetNominalsOfContinuousStates ( modelInstantiated \
| modelEventMode | modelContinuousTimeMode \
| modelTerminated | modelError)

// ---------------------------------------------------------------------------
// Function calls allowed state masks for Co-simulation
// ---------------------------------------------------------------------------
#define MASK_fmi3SetRealInputDerivatives (modelInstantiated | modelInitializationMode \
| modelStepComplete)
#define MASK_fmi3GetRealOutputDerivatives (modelStepComplete | modelStepFailed | modelStepCanceled \
| modelTerminated | modelError)
#define MASK_fmi3DoStep                  modelStepComplete
#define MASK_fmi3CancelStep              modelStepInProgress
#define MASK_fmi3GetStatus               (modelStepComplete | modelStepInProgress | modelStepFailed \
| modelTerminated)
#define MASK_fmi3GetRealStatus           MASK_fmi3GetStatus
#define MASK_fmi3GetIntegerStatus        MASK_fmi3GetStatus
#define MASK_fmi3GetBooleanStatus        MASK_fmi3GetStatus
#define MASK_fmi3GetStringStatus         MASK_fmi3GetStatus

// ---------------------------------------------------------------------------
// Private helpers used below to validate function arguments
// ---------------------------------------------------------------------------

#define NOT_IMPLEMENTED ModelInstance *comp = (ModelInstance *)instance; \
    logError(comp, "Function is not implemented."); \
    return fmi3Error;

static fmi3Status unsupportedFunction(fmi3Instance instance, const char *fName, int statesExpected) {
    ModelInstance *comp = (ModelInstance *)instance;
    if (invalidState(comp, fName, statesExpected))
        return fmi3Error;
    logError(comp, "%s: Function not implemented.", fName);
    return fmi3Error;
}

/***************************************************
 Common Functions
 ****************************************************/

const char* fmi3GetVersion() {
    return fmi3Version;
}

fmi3Status fmi3SetDebugLogging(fmi3Instance instance, fmi3Boolean loggingOn, size_t nCategories, const fmi3String categories[]) {
    
    ModelInstance *comp = (ModelInstance *)instance;
    
    if (invalidState(comp, "fmi3SetDebugLogging", MASK_fmi3SetDebugLogging))
        return fmi3Error;
    
    return setDebugLogging(comp, loggingOn, nCategories, categories);
}

fmi3Instance fmi3InstantiateBasicCoSimulation(
                             fmi3String                     instanceName,
                             fmi3String                     instantiationToken,
                             fmi3String                     resourceLocation,
                             fmi3Boolean                    visible,
                             fmi3Boolean                    loggingOn,
                             fmi3Boolean                    intermediateVariableGetRequired,
                             fmi3Boolean                    intermediateInternalVariableGetRequired,
                             fmi3Boolean                    intermediateVariableSetRequired,
                             fmi3InstanceEnvironment        instanceEnvironment,
                             fmi3CallbackLogMessage         logMessage,
                             fmi3CallbackAllocateMemory     allocateMemory,
                             fmi3CallbackFreeMemory         freeMemory,
                             fmi3CallbackIntermediateUpdate intermediateUpdate)
{

    return createModelInstance(
        (loggerType)logMessage,
        (allocateMemoryType)allocateMemory,
        (freeMemoryType)freeMemory,
        instanceEnvironment,
        instanceName,
        instantiationToken,
        resourceLocation,
        loggingOn,
        fmi3BasicCoSimulation
    );
}

fmi3Instance fmi3InstantiateModelExchange (
                              fmi3String                 instanceName,
                              fmi3String                 instantiationToken,
                              fmi3String                 resourceLocation,
                              fmi3Boolean                visible,
                              fmi3Boolean                loggingOn,
                              fmi3InstanceEnvironment    instanceEnvironment,
                              fmi3CallbackLogMessage     logMessage,
                              fmi3CallbackAllocateMemory allocateMemory,
                              fmi3CallbackFreeMemory     freeMemory)
{
    return createModelInstance(
        (loggerType)logMessage,
        (allocateMemoryType)allocateMemory,
        (freeMemoryType)freeMemory,
        instanceEnvironment,
        instanceName,
        instantiationToken,
        resourceLocation,
        loggingOn,
        fmi3ModelExchange
    );
}

void fmi3FreeInstance(fmi3Instance instance) {
    
    ModelInstance *comp = (ModelInstance *)instance;
    
    if (!comp) return;
    
    if (invalidState(comp, "fmi3FreeInstance", MASK_fmi3FreeInstance))
        return;
    
    freeModelInstance(comp);
}

fmi3Status fmi3SetupExperiment(fmi3Instance instance, fmi3Boolean toleranceDefined, fmi3Float64 tolerance,
                            fmi3Float64 startTime, fmi3Boolean stopTimeDefined, fmi3Float64 stopTime) {

    // ignore arguments: stopTimeDefined, stopTime
    ModelInstance *comp = (ModelInstance *)instance;

    if (invalidState(comp, "fmi3SetupExperiment", MASK_fmi3SetupExperiment))
        return fmi3Error;

    comp->time = startTime;

    return fmi3OK;
}

fmi3Status fmi3EnterInitializationMode(fmi3Instance instance) {
    ModelInstance *comp = (ModelInstance *)instance;
    if (invalidState(comp, "fmi3EnterInitializationMode", MASK_fmi3EnterInitializationMode))
        return fmi3Error;
    comp->state = modelInitializationMode;
    return fmi3OK;
}

fmi3Status fmi3ExitInitializationMode(fmi3Instance instance) {
    ModelInstance *comp = (ModelInstance *)instance;
    if (invalidState(comp, "fmi3ExitInitializationMode", MASK_fmi3ExitInitializationMode))
        return fmi3Error;

    // if values were set and no fmi3GetXXX triggered update before,
    // ensure calculated values are updated now
    if (comp->isDirtyValues) {
        calculateValues(comp);
        comp->isDirtyValues = false;
    }

    if (comp->type == fmi3ModelExchange) {
        comp->state = modelEventMode;
        comp->isNewEventIteration = fmi3True;
    } else {
        comp->state = modelStepComplete;
    }

    return fmi3OK;
}

fmi3Status fmi3EnterEventMode(fmi3Instance instance,
                              fmi3Boolean inputEvent,
                              fmi3Boolean stepEvent,
                              const fmi3Int32 rootsFound[],
                              size_t nEventIndicators,
                              fmi3Boolean timeEvent) {
    ModelInstance *comp = (ModelInstance *)instance;
    if (invalidState(comp, "fmi3EnterEventMode", MASK_fmi3EnterEventMode))
        return fmi3Error;
    comp->state = modelEventMode;
    comp->isNewEventIteration = fmi3True;
    return fmi3OK;
}

fmi3Status fmi3Terminate(fmi3Instance instance) {
    ModelInstance *comp = (ModelInstance *)instance;
    if (invalidState(comp, "fmi3Terminate", MASK_fmi3Terminate))
        return fmi3Error;
    comp->state = modelTerminated;
    return fmi3OK;
}

fmi3Status fmi3Reset(fmi3Instance instance) {
    ModelInstance* comp = (ModelInstance *)instance;
    if (invalidState(comp, "fmi3Reset", MASK_fmi3Reset))
        return fmi3Error;
    comp->state = modelInstantiated;
    setStartValues(comp);
    comp->isDirtyValues = true;
    return fmi3OK;
}

fmi3Status fmi3GetFloat32(fmi3Instance instance,
                          const fmi3ValueReference valueReferences[], size_t nValueReferences,
                          fmi3Float32 values[], size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetFloat64(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3Float64 value[], size_t nValues) {

    ModelInstance *comp = (ModelInstance *)instance;

    if (invalidState(comp, "fmi3GetReal", MASK_fmi3GetReal))
        return fmi3Error;

    if (nvr > 0 && nullPointer(comp, "fmi3GetReal", "vr[]", vr))
        return fmi3Error;

    if (nvr > 0 && nullPointer(comp, "fmi3GetReal", "value[]", value))
        return fmi3Error;

    if (nvr > 0 && comp->isDirtyValues) {
        calculateValues(comp);
        comp->isDirtyValues = false;
    }

    GET_VARIABLES(Float64)
}

fmi3Status fmi3GetInt8(fmi3Instance instance,
                       const fmi3ValueReference valueReferences[], size_t nValueReferences,
                       fmi3Int8 values[], size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetUInt8(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[], size_t nValueReferences,
                        fmi3UInt8 values[], size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetInt16(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[], size_t nValueReferences,
                        fmi3Int16 values[], size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetUInt16(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[], size_t nValueReferences,
                         fmi3UInt16 values[], size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetInt32(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3Int32 value[], size_t nValues) {

    ModelInstance *comp = (ModelInstance *)instance;

    if (invalidState(comp, "fmi3GetInteger", MASK_fmi3GetInteger))
        return fmi3Error;

    if (nvr > 0 && nullPointer(comp, "fmi3GetInteger", "vr[]", vr))
            return fmi3Error;

    if (nvr > 0 && nullPointer(comp, "fmi3GetInteger", "value[]", value))
            return fmi3Error;

    if (nvr > 0 && comp->isDirtyValues) {
        calculateValues(comp);
        comp->isDirtyValues = false;
    }

    GET_VARIABLES(Int32)
}

fmi3Status fmi3GetUInt32(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[], size_t nValueReferences,
                         fmi3UInt32 values[], size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetInt64(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[], size_t nValueReferences,
                        fmi3Int64 values[], size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetUInt64(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[], size_t nValueReferences,
                         fmi3UInt64 values[], size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetBoolean(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3Boolean value[], size_t nValues) {

    ModelInstance *comp = (ModelInstance *)instance;

    if (invalidState(comp, "fmi3GetBoolean", MASK_fmi3GetBoolean))
        return fmi3Error;

    if (nvr > 0 && nullPointer(comp, "fmi3GetBoolean", "vr[]", vr))
            return fmi3Error;

    if (nvr > 0 && nullPointer(comp, "fmi3GetBoolean", "value[]", value))
            return fmi3Error;

    if (nvr > 0 && comp->isDirtyValues) {
        calculateValues(comp);
        comp->isDirtyValues = false;
    }

    GET_BOOLEAN_VARIABLES
}

fmi3Status fmi3GetString(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, fmi3String value[], size_t nValues) {

    ModelInstance *comp = (ModelInstance *)instance;

    if (invalidState(comp, "fmi3GetString", MASK_fmi3GetString))
        return fmi3Error;

    if (nvr>0 && nullPointer(comp, "fmi3GetString", "vr[]", vr))
            return fmi3Error;

    if (nvr>0 && nullPointer(comp, "fmi3GetString", "value[]", value))
            return fmi3Error;

    if (nvr > 0 && comp->isDirtyValues) {
        calculateValues(comp);
        comp->isDirtyValues = false;
    }

    GET_VARIABLES(String)
}

fmi3Status fmi3GetBinary(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, size_t size[], fmi3Binary value[], size_t nValues) {

    ModelInstance *comp = (ModelInstance *)instance;

    Status status = OK;

    for (int i = 0; i < nvr; i++) {
        size_t index = 0;
        Status s = getBinary(comp, vr[i], size, value, &index);
        status = max(status, s);
        if (status > Warning) return status;
    }

    return status;
}

fmi3Status fmi3SetFloat32(fmi3Instance instance,
                          const fmi3ValueReference valueReferences[], size_t nValueReferences,
                          const fmi3Float32 values[], size_t nValues) {
    NOT_IMPLEMENTED
}


fmi3Status fmi3SetFloat64(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3Float64 value[], size_t nValues) {

    ModelInstance *comp = (ModelInstance *)instance;

    if (invalidState(comp, "fmi3SetReal", MASK_fmi3SetReal))
        return fmi3Error;

    if (nvr > 0 && nullPointer(comp, "fmi3SetReal", "vr[]", vr))
        return fmi3Error;

    if (nvr > 0 && nullPointer(comp, "fmi3SetReal", "value[]", value))
        return fmi3Error;

    SET_VARIABLES(Float64)
}

fmi3Status fmi3SetInt8(fmi3Instance instance,
                       const fmi3ValueReference valueReferences[], size_t nValueReferences,
                       const fmi3Int8 values[], size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3SetUInt8(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[], size_t nValueReferences,
                        const fmi3UInt8 values[], size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3SetInt16(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[], size_t nValueReferences,
                        const fmi3Int16 values[], size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3SetUInt16(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[], size_t nValueReferences,
                         const fmi3UInt16 values[], size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3SetInt32(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3Int32 value[], size_t nValues) {

    ModelInstance *comp = (ModelInstance *)instance;

    if (invalidState(comp, "fmi3SetInteger", MASK_fmi3SetInteger))
        return fmi3Error;

    if (nvr > 0 && nullPointer(comp, "fmi3SetInteger", "vr[]", vr))
        return fmi3Error;

    if (nvr > 0 && nullPointer(comp, "fmi3SetInteger", "value[]", value))
        return fmi3Error;

    SET_VARIABLES(Int32)
}

fmi3Status fmi3SetUInt32(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[], size_t nValueReferences,
                         const fmi3UInt32 values[], size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3SetInt64(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[], size_t nValueReferences,
                        const fmi3Int64 values[], size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3SetUInt64(fmi3Instance instance,
                         const fmi3ValueReference valueReferences[], size_t nValueReferences,
                         const fmi3UInt64 values[], size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3SetBoolean(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3Boolean value[], size_t nValues) {

    ModelInstance *comp = (ModelInstance *)instance;

    if (invalidState(comp, "fmi3SetBoolean", MASK_fmi3SetBoolean))
        return fmi3Error;

    if (nvr>0 && nullPointer(comp, "fmi3SetBoolean", "vr[]", vr))
        return fmi3Error;

    if (nvr>0 && nullPointer(comp, "fmi3SetBoolean", "value[]", value))
        return fmi3Error;

    SET_BOOLEAN_VARIABLES
}

fmi3Status fmi3SetString(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const fmi3String value[], size_t nValues) {

    ModelInstance *comp = (ModelInstance *)instance;

    if (invalidState(comp, "fmi3SetString", MASK_fmi3SetString))
        return fmi3Error;

    if (nvr>0 && nullPointer(comp, "fmi3SetString", "vr[]", vr))
        return fmi3Error;

    if (nvr>0 && nullPointer(comp, "fmi3SetString", "value[]", value))
        return fmi3Error;

    SET_VARIABLES(String)
}

fmi3Status fmi3SetBinary(fmi3Instance instance, const fmi3ValueReference vr[], size_t nvr, const size_t size[], const fmi3Binary value[], size_t nValues) {

    ModelInstance *comp = (ModelInstance *)instance;

    Status status = OK;

    for (int i = 0; i < nvr; i++) {
        size_t index = 0;
        Status s = setBinary(comp, vr[i], size, value, &index);
        status = max(status, s);
        if (status > Warning) return status;
    }

    return status;
}

fmi3Status fmi3GetNumberOfVariableDependencies(fmi3Instance instance,
                                               fmi3ValueReference valueReference,
                                               size_t* nDependencies) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetVariableDependencies(fmi3Instance instance,
                                       fmi3ValueReference dependent,
                                       size_t elementIndicesOfDependent[],
                                       fmi3ValueReference independents[],
                                       size_t elementIndicesOfIndependents[],
                                       fmi3DependencyKind dependencyKinds[],
                                       size_t nDependencies) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetFMUState(fmi3Instance instance, fmi3FMUState* FMUState) {
    return unsupportedFunction(instance, "fmi3GetFMUState", MASK_fmi3GetFMUState);
}
fmi3Status fmi3SetFMUState(fmi3Instance instance, fmi3FMUState FMUState) {
    return unsupportedFunction(instance, "fmi3SetFMUState", MASK_fmi3SetFMUState);
}
fmi3Status fmi3FreeFMUState(fmi3Instance instance, fmi3FMUState* FMUState) {
    return unsupportedFunction(instance, "fmi3FreeFMUState", MASK_fmi3FreeFMUState);
}
fmi3Status fmi3SerializedFMUStateSize(fmi3Instance instance, fmi3FMUState FMUState, size_t *size) {
    return unsupportedFunction(instance, "fmi3SerializedFMUStateSize", MASK_fmi3SerializedFMUStateSize);
}
fmi3Status fmi3SerializeFMUState(fmi3Instance instance, fmi3FMUState FMUState, fmi3Byte serializedState[], size_t size) {
    return unsupportedFunction(instance, "fmi3SerializeFMUState", MASK_fmi3SerializeFMUState);
}
fmi3Status fmi3DeSerializeFMUState (fmi3Instance instance, const fmi3Byte serializedState[], size_t size,
                                    fmi3FMUState* FMUState) {
    return unsupportedFunction(instance, "fmi3DeSerializeFMUState", MASK_fmi3DeSerializeFMUState);
}

fmi3Status fmi3GetDirectionalDerivative(fmi3Instance instance, const fmi3ValueReference unknowns[], size_t nUnknowns, const fmi3ValueReference knowns[], size_t nKnowns, const fmi3Float64 deltaKnowns[], size_t nDeltaKnowns, fmi3Float64 deltaUnknowns[], size_t nDeltaOfUnknowns) {
    
    if (invalidState(instance, "fmi3GetDirectionalDerivative", MASK_fmi3GetDirectionalDerivative))
        return fmi3Error;
    
    // TODO: check value references
    // TODO: assert nUnknowns == nDeltaOfUnknowns
    // TODO: assert nKnowns == nDeltaKnowns

    ModelInstance *comp = (ModelInstance *)instance;
    Status status = OK;
    
    for (int i = 0; i < nUnknowns; i++) {
        deltaUnknowns[i] = 0;
        for (int j = 0; j < nKnowns; j++) {
            double partialDerivative = 0;
            Status s = getPartialDerivative(comp, unknowns[i], knowns[j], &partialDerivative);
            status = max(status, s);
            if (status > Warning) return status;
            deltaUnknowns[i] += partialDerivative * deltaKnowns[j];
        }
    }
    
    return fmi3OK;
}

fmi3Status fmi3EnterConfigurationMode(fmi3Instance instance) {
    ModelInstance *comp = (ModelInstance *)instance;
    logError(comp, "fmi3EnterConfigurationMode() is not supported.");
    return fmi3Error;
}

fmi3Status fmi3ExitConfigurationMode(fmi3Instance instance) {
    ModelInstance *comp = (ModelInstance *)instance;
    logError(comp, "fmi3ExitConfigurationMode() is not supported.");
    return fmi3Error;
}

fmi3Status fmi3SetClock(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        const fmi3Clock values[],
                        const fmi3Boolean subactive[],
                        size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetClock(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        fmi3Clock values[],
                        size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetIntervalDecimal(fmi3Instance instance,
                                  const fmi3ValueReference valueReferences[],
                                  size_t nValueReferences,
                                  fmi3Float64 interval[],
                                  size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3SetIntervalDecimal(fmi3Instance instance,
                                  const fmi3ValueReference valueReferences[],
                                  size_t nValueReferences,
                                  const fmi3Float64 interval[],
                                  size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetIntervalFraction(fmi3Instance instance,
                                   const fmi3ValueReference valueReferences[],
                                   size_t nValueReferences,
                                   fmi3UInt64 intervalCounter[],
                                   fmi3UInt64 resolution[],
                                   size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3SetIntervalFraction(fmi3Instance instance,
                                   const fmi3ValueReference valueReferences[],
                                   size_t nValueReferences,
                                   const fmi3UInt64 intervalCounter[],
                                   const fmi3UInt64 resolution[],
                                   size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3NewDiscreteStates(fmi3Instance instance,
                                 fmi3Boolean *newDiscreteStatesNeeded,
                                 fmi3Boolean *terminateSimulation,
                                 fmi3Boolean *nominalsOfContinuousStatesChanged,
                                 fmi3Boolean *valuesOfContinuousStatesChanged,
                                 fmi3Boolean *nextEventTimeDefined,
                                 fmi3Float64 *nextEventTime) {

    ModelInstance *comp = (ModelInstance *)instance;

    if (invalidState(comp, "fmi3NewDiscreteStates", MASK_fmi3NewDiscreteStates))
        return fmi3Error;
    
    comp->newDiscreteStatesNeeded = fmi3False;
    comp->terminateSimulation = fmi3False;
    comp->nominalsOfContinuousStatesChanged = fmi3False;
    comp->valuesOfContinuousStatesChanged = fmi3False;
    
    eventUpdate(comp);
    
    comp->isNewEventIteration = false;
    
    // copy internal eventInfo of component to output eventInfo
    *newDiscreteStatesNeeded           = comp->newDiscreteStatesNeeded;
    *terminateSimulation               = comp->terminateSimulation;
    *nominalsOfContinuousStatesChanged = comp->nominalsOfContinuousStatesChanged;
    *valuesOfContinuousStatesChanged   = comp->valuesOfContinuousStatesChanged;
    *nextEventTimeDefined              = comp->nextEventTimeDefined;
    *nextEventTime                     = comp->nextEventTime;
    
    return fmi3OK;
}

/***************************************************
 Functions for FMI3 for Model Exchange
 ****************************************************/

fmi3Status fmi3EnterContinuousTimeMode(fmi3Instance instance) {
    ModelInstance *comp = (ModelInstance *)instance;
    if (invalidState(comp, "fmi3EnterContinuousTimeMode", MASK_fmi3EnterContinuousTimeMode))
        return fmi3Error;
    comp->state = modelContinuousTimeMode;
    return fmi3OK;
}

fmi3Status fmi3CompletedIntegratorStep(fmi3Instance instance, fmi3Boolean noSetFMUStatePriorToCurrentPoint,
                                       fmi3Boolean *enterEventMode, fmi3Boolean *terminateSimulation) {
    ModelInstance *comp = (ModelInstance *)instance;
    if (invalidState(comp, "fmi3CompletedIntegratorStep", MASK_fmi3CompletedIntegratorStep))
        return fmi3Error;
    if (nullPointer(comp, "fmi3CompletedIntegratorStep", "enterEventMode", enterEventMode))
        return fmi3Error;
    if (nullPointer(comp, "fmi3CompletedIntegratorStep", "terminateSimulation", terminateSimulation))
        return fmi3Error;
    *enterEventMode = fmi3False;
    *terminateSimulation = fmi3False;
    return fmi3OK;
}

/* Providing independent variables and re-initialization of caching */
fmi3Status fmi3SetTime(fmi3Instance instance, fmi3Float64 time) {
    ModelInstance *comp = (ModelInstance *)instance;
    if (invalidState(comp, "fmi3SetTime", MASK_fmi3SetTime))
        return fmi3Error;
    comp->time = time;
    return fmi3OK;
}

fmi3Status fmi3SetContinuousStates(fmi3Instance instance, const fmi3Float64 x[], size_t nx){
    
    ModelInstance *comp = (ModelInstance *)instance;
    
    if (invalidState(comp, "fmi3SetContinuousStates", MASK_fmi3SetContinuousStates))
        return fmi3Error;
    
    if (invalidNumber(comp, "fmi3SetContinuousStates", "nx", nx, NUMBER_OF_STATES))
        return fmi3Error;
    
    if (nullPointer(comp, "fmi3SetContinuousStates", "x[]", x))
        return fmi3Error;
    
    setContinuousStates(comp, x, nx);
    
    return fmi3OK;
}

/* Evaluation of the model equations */
fmi3Status fmi3GetDerivatives(fmi3Instance instance, fmi3Float64 derivatives[], size_t nx) {
    
    ModelInstance* comp = (ModelInstance *)instance;
    
    if (invalidState(comp, "fmi3GetDerivatives", MASK_fmi3GetDerivatives))
        return fmi3Error;
    
    if (invalidNumber(comp, "fmi3GetDerivatives", "nx", nx, NUMBER_OF_STATES))
        return fmi3Error;
    
    if (nullPointer(comp, "fmi3GetDerivatives", "derivatives[]", derivatives))
        return fmi3Error;
    
    getDerivatives(comp, derivatives, nx);
    
    return fmi3OK;
}

fmi3Status fmi3GetEventIndicators(fmi3Instance instance, fmi3Float64 eventIndicators[], size_t ni) {
    
#if NUMBER_OF_EVENT_INDICATORS > 0
    ModelInstance *comp = (ModelInstance *)instance;
    
    if (invalidState(comp, "fmi3GetEventIndicators", MASK_fmi3GetEventIndicators))
        return fmi3Error;
    
    if (invalidNumber(comp, "fmi3GetEventIndicators", "ni", ni, NUMBER_OF_EVENT_INDICATORS))
        return fmi3Error;
    
    getEventIndicators(comp, eventIndicators, ni);
#else
    if (ni > 0) return fmi3Error;
#endif
    return fmi3OK;
}

fmi3Status fmi3GetContinuousStates(fmi3Instance instance, fmi3Float64 states[], size_t nx) {
    
    ModelInstance *comp = (ModelInstance *)instance;
    
    if (invalidState(comp, "fmi3GetContinuousStates", MASK_fmi3GetContinuousStates))
        return fmi3Error;
    
    if (invalidNumber(comp, "fmi3GetContinuousStates", "nx", nx, NUMBER_OF_STATES))
        return fmi3Error;
    
    if (nullPointer(comp, "fmi3GetContinuousStates", "states[]", states))
        return fmi3Error;
    
    getContinuousStates(comp, states, nx);
    
    return fmi3OK;
}

fmi3Status fmi3GetNominalsOfContinuousStates(fmi3Instance instance, fmi3Float64 x_nominal[], size_t nx) {
    int i;
    ModelInstance *comp = (ModelInstance *)instance;
    if (invalidState(comp, "fmi3GetNominalsOfContinuousStates", MASK_fmi3GetNominalsOfContinuousStates))
        return fmi3Error;
    if (invalidNumber(comp, "fmi3GetNominalContinuousStates", "nx", nx, NUMBER_OF_STATES))
        return fmi3Error;
    if (nullPointer(comp, "fmi3GetNominalContinuousStates", "x_nominal[]", x_nominal))
        return fmi3Error;
    for (i = 0; i < nx; i++)
        x_nominal[i] = 1;
    return fmi3OK;
}

fmi3Status fmi3GetNumberOfEventIndicators(fmi3Instance instance, size_t* nz) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetNumberOfContinuousStates(fmi3Instance instance, size_t* nx) {
    NOT_IMPLEMENTED
}

/***************************************************
 Functions for FMI3 for Co-Simulation
 ****************************************************/

fmi3Status fmi3EnterStepMode(fmi3Instance instance) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3SetInputDerivatives(fmi3Instance instance,
                                   const fmi3ValueReference valueReferences[],
                                   size_t nValueReferences,
                                   const fmi3Int32 orders[],
                                   const fmi3Float64 values[],
                                   size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetOutputDerivatives(fmi3Instance instance,
                                    const fmi3ValueReference valueReferences[],
                                    size_t nValueReferences,
                                    const fmi3Int32 orders[],
                                    fmi3Float64 values[],
                                    size_t nValues) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3DoStep(fmi3Instance instance,
                      fmi3Float64 currentCommunicationPoint,
                      fmi3Float64 communicationStepSize,
                      fmi3Boolean noSetFMUStatePriorToCurrentPoint,
                      fmi3Boolean* earlyReturn) {

    ModelInstance *comp = (ModelInstance *)instance;

    if (communicationStepSize <= 0) {
        logError(comp, "fmi3DoStep: communication step size must be > 0 but was %g.", communicationStepSize);
        comp->state = modelError;
        return fmi3Error;
    }

    return doStep(comp, currentCommunicationPoint, currentCommunicationPoint + communicationStepSize);
}

fmi3Status fmi3ActivateModelPartition(fmi3Instance instance,
                                      fmi3ValueReference clockReference,
                                      size_t clockElementIndex,
                                      fmi3Float64 activationTime) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3DoEarlyReturn(fmi3Instance instance, fmi3Float64 earlyReturnTime) {
    NOT_IMPLEMENTED
}

fmi3Status fmi3GetDoStepDiscardedStatus(fmi3Instance instance, fmi3Boolean* terminate, fmi3Float64* lastSuccessfulTime) {
    return fmi3Error;
}
