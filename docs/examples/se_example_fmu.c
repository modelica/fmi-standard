
#include <stdio.h>
#include <math.h>
#include <stdlib.h>


#include "fmi3Functions.h"
#include "se_example_fmu.h"

#define fmi3Version "3.0-alpha.6"

/***************************************************
 Common Functions
 ****************************************************/

const char* fmi3GetVersion() {
    return fmi3Version;
}

fmi3Status fmi3SetDebugLogging(fmi3Instance instance,
							   fmi3Boolean loggingOn,
							   size_t nCategories,
							   const fmi3String categories[]) {
    return fmi3OK;
}

fmi3Instance fmi3InstantiateScheduledExecution(
    fmi3String                     instanceName,
    fmi3String                     instantiationToken,
    fmi3String                     resourceLocation,
    fmi3Boolean                    visible,
    fmi3Boolean                    loggingOn,
    const fmi3ValueReference       requiredIntermediateVariables[],
    size_t                         nRequiredIntermediateVariables,
    fmi3InstanceEnvironment        instanceEnvironment,
    fmi3CallbackLogMessage         logMessage,
    fmi3CallbackIntermediateUpdate intermediateUpdate,
    fmi3CallbackLockPreemption     lockPreemption,
    fmi3CallbackUnlockPreemption   unlockPreemption) {
		
		ModelInstance *comp = NULL;
		
		comp = (ModelInstance *)calloc(1, sizeof(ModelInstance));
		
		
		if (comp) {
			comp->instanceEnvironment = instanceEnvironment;
			comp->logger = logMessage;
			comp->callbackIntermediateUpdate = intermediateUpdate;
			comp->lockPreemption = lockPreemption;
			comp->unlockPreemption = unlockPreemption;
		}
		return comp;
	
}

void fmi3FreeInstance(fmi3Instance instance) {

    ModelInstance *comp = (ModelInstance *)instance;

    if (!comp) return;

    free(comp);
}


fmi3Status fmi3EnterInitializationMode(fmi3Instance instance,
                                                   fmi3Boolean toleranceDefined,
                                                   fmi3Float64 tolerance,
                                                   fmi3Float64 startTime,
                                                   fmi3Boolean stopTimeDefined,
                                                   fmi3Float64 stopTime) {

    return fmi3OK;
}

fmi3Status fmi3ExitInitializationMode(fmi3Instance instance) {

    return fmi3OK;
}

fmi3Status fmi3EnterEventMode(fmi3Instance instance,
                              fmi3Boolean inputEvent,
                              fmi3Boolean stepEvent,
                              const fmi3Int32 rootsFound[],
                              size_t nEventIndicators,
                              fmi3Boolean timeEvent) {

    return fmi3OK;
}

fmi3Status fmi3Terminate(fmi3Instance instance) {
    return fmi3OK;
}

fmi3Status fmi3Reset(fmi3Instance instance) {
    return fmi3OK;
}

fmi3Status fmi3GetFloat32(fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  fmi3Float32 values[],
						  size_t nValues) {
    return fmi3OK;
}


fmi3Status fmi3GetInt8(fmi3Instance instance,
                       const fmi3ValueReference valueReferences[], size_t nValueReferences,
                       fmi3Int8 values[], size_t nValues) {
	return fmi3OK;
}

fmi3Status fmi3GetUInt8(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[], size_t nValueReferences,
                        fmi3UInt8 values[], size_t nValues) {
	return fmi3OK;
}

fmi3Status fmi3GetInt16(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[], size_t nValueReferences,
                        fmi3Int16 values[], size_t nValues) {
	return fmi3OK;
}

fmi3Status fmi3GetUInt16 (fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  fmi3UInt16 values[],
						  size_t nValues){
	return fmi3OK;
}


fmi3Status fmi3GetInt32  (fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  fmi3Int32 values[],
						  size_t nValues){
	return fmi3OK;
}


fmi3Status fmi3GetUInt32 (fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  fmi3UInt32 values[],
						  size_t nValues){
	return fmi3OK;
}


fmi3Status fmi3GetInt64	 (fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  fmi3Int64 values[],
						  size_t nValues){
	return fmi3OK;
}


fmi3Status fmi3GetUInt64 (fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  fmi3UInt64 values[],
						  size_t nValues){
	return fmi3OK;
}


fmi3Status fmi3GetBoolean(fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  fmi3Boolean values[],
						  size_t nValues){
	return fmi3OK;
}


fmi3Status fmi3GetString (fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  fmi3String values[],
						  size_t nValues){
	return fmi3OK;
}


fmi3Status fmi3GetBinary (fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  size_t sizes[],
						  fmi3Binary values[],
						  size_t nValues){
	return fmi3OK;
}


fmi3Status fmi3SetFloat32(fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  const fmi3Float32 values[],
						  size_t nValues){
	return fmi3OK;
}


fmi3Status fmi3SetInt8   (fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  const fmi3Int8 values[],
						  size_t nValues){
	return fmi3OK;
}

fmi3Status fmi3SetUInt8  (fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  const fmi3UInt8 values[],
						  size_t nValues){
	return fmi3OK;
}

fmi3Status fmi3SetInt16  (fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  const fmi3Int16 values[],
						  size_t nValues){
	return fmi3OK;
}

fmi3Status fmi3SetUInt16 (fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  const fmi3UInt16 values[],
						  size_t nValues){
	return fmi3OK;
}

fmi3Status fmi3SetInt32  (fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  const fmi3Int32 values[],
						  size_t nValues){
	return fmi3OK;
}
									  
fmi3Status fmi3SetUInt32 (fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  const fmi3UInt32 values[],
						  size_t nValues);

fmi3Status fmi3SetInt64  (fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  const fmi3Int64 values[],
						  size_t nValues){
	return fmi3OK;
}

fmi3Status fmi3SetUInt64 (fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  const fmi3UInt64 values[],
						  size_t nValues){
	return fmi3OK;
}

fmi3Status fmi3SetBoolean(fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  const fmi3Boolean values[],
						  size_t nValues){
	return fmi3OK;
}

fmi3Status fmi3SetString (fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  const fmi3String values[],
						  size_t nValues){
	return fmi3OK;
}

fmi3Status fmi3SetBinary (fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  const size_t sizes[],
						  const fmi3Binary values[],
						  size_t nValues){
	return fmi3OK;
}

fmi3Status fmi3SetClock  (fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  const fmi3Clock values[],
						  size_t nValues){
	return fmi3OK;
}

fmi3Status fmi3GetNumberOfVariableDependencies(fmi3Instance instance,
                                               fmi3ValueReference valueReference,
                                               size_t* nDependencies) {
	return fmi3OK;
}

fmi3Status fmi3GetVariableDependencies(fmi3Instance instance,
                                       fmi3ValueReference dependent,
                                       size_t elementIndicesOfDependent[],
                                       fmi3ValueReference independents[],
                                       size_t elementIndicesOfIndependents[],
                                       fmi3DependencyKind dependencyKinds[],
                                       size_t nDependencies) {
	return fmi3OK;
}

fmi3Status fmi3GetFMUState(fmi3Instance instance, fmi3FMUState* FMUState) {
	return fmi3OK;
}
fmi3Status fmi3SetFMUState(fmi3Instance instance, fmi3FMUState FMUState) {
	return fmi3OK;
}
fmi3Status fmi3FreeFMUState(fmi3Instance instance, fmi3FMUState* FMUState) {
	return fmi3OK;
}
fmi3Status fmi3SerializedFMUStateSize(fmi3Instance instance, fmi3FMUState FMUState, size_t *size) {
	return fmi3OK;
}
fmi3Status fmi3SerializeFMUState(fmi3Instance instance, fmi3FMUState FMUState, fmi3Byte serializedState[], size_t size) {
	return fmi3OK;
}
fmi3Status fmi3DeSerializeFMUState (fmi3Instance instance, const fmi3Byte serializedState[], size_t size,
                                    fmi3FMUState* FMUState) {
	return fmi3OK;
}

fmi3Status fmi3GetDirectionalDerivative(fmi3Instance instance,
										const fmi3ValueReference unknowns[],
										size_t nUnknowns,
										const fmi3ValueReference knowns[],
										size_t nKnowns,
										const fmi3Float64 seed[],
										size_t nSeed,
										fmi3Float64 sensitivity[],
										size_t nSensitivity){

    return fmi3OK;
}

fmi3Status fmi3GetAdjointDerivative(fmi3Instance instance,
									const fmi3ValueReference unknowns[],
									size_t nUnknowns,
									const fmi3ValueReference knowns[],
									size_t nKnowns,
									const fmi3Float64 seed[],
									size_t nSeed,
									fmi3Float64 sensitivity[],
									size_t nSensitivity){

    return fmi3OK;
}


fmi3Status fmi3EnterConfigurationMode(fmi3Instance instance) {
    return fmi3Error;
}

fmi3Status fmi3ExitConfigurationMode(fmi3Instance instance) {
    return fmi3Error;
}




fmi3Status fmi3SetIntervalDecimal(fmi3Instance instance,
								  const fmi3ValueReference valueReferences[],
								  size_t nValueReferences,
								  const fmi3Float64 interval[],
								  size_t nValues) {
    return fmi3OK;
}

fmi3Status fmi3GetIntervalFraction(fmi3Instance instance,
								   const fmi3ValueReference valueReferences[],
								   size_t nValueReferences,
								   fmi3UInt64 intervalCounter[],
								   fmi3UInt64 resolution[],
								   fmi3IntervalQualifier qualifier[],
								   size_t nValues) {
    return fmi3OK;
}

fmi3Status fmi3SetIntervalFraction(fmi3Instance instance,
								   const fmi3ValueReference valueReferences[],
								   size_t nValueReferences,
								   const fmi3UInt64 intervalCounter[],
								   const fmi3UInt64 resolution[],
								   size_t nValues) {
    return fmi3OK;
}


fmi3Status fmi3EnterContinuousTimeMode(fmi3Instance instance) {
    return fmi3Error;
}

fmi3Status fmi3CompletedIntegratorStep (fmi3Instance instance,
										fmi3Boolean noSetFMUStatePriorToCurrentPoint,
										fmi3Boolean *enterEventMode,
										fmi3Boolean *terminateSimulation) {
    return fmi3Error;
}

fmi3Status fmi3SetTime (fmi3Instance instance,
						fmi3Float64 time) {
    return fmi3Error;
}

fmi3Status fmi3SetContinuousStates (fmi3Instance instance,
									const fmi3Float64 x[],
									size_t nx){
    return fmi3Error;
}

/* Evaluation of the model equations */
fmi3Status fmi3GetDerivatives  (fmi3Instance instance,
								fmi3Float64 derivatives[],
								size_t nx) {
    return fmi3Error;
}

fmi3Status fmi3GetEventIndicators  (fmi3Instance instance,
									fmi3Float64 eventIndicators[],
									size_t ni) {
    return fmi3Error;
}

fmi3Status fmi3GetContinuousStates (fmi3Instance instance,
									fmi3Float64 states[],
									size_t nx) {
    return fmi3Error;
}

fmi3Status fmi3GetNominalsOfContinuousStates   (fmi3Instance instance,
												fmi3Float64 x_nominal[],
												size_t nx) {
    return fmi3Error;
}

fmi3Status fmi3GetNumberOfEventIndicators  (fmi3Instance instance,
											size_t* nz) {
    return fmi3Error;
}

fmi3Status fmi3GetNumberOfContinuousStates (fmi3Instance instance,
											size_t* nx) {
    return fmi3Error;
}


fmi3Status fmi3EnterStepMode(fmi3Instance instance) {
    return fmi3Error;
}

fmi3Status fmi3SetInputDerivatives(fmi3Instance instance,
                                   const fmi3ValueReference valueReferences[],
                                   size_t nValueReferences,
                                   const fmi3Int32 orders[],
                                   const fmi3Float64 values[],
                                   size_t nValues) {
    return fmi3Error;
}

fmi3Status fmi3GetOutputDerivatives(fmi3Instance instance,
									const fmi3ValueReference valueReferences[],
									size_t nValueReferences,
									const fmi3Int32 orders[],
									fmi3Float64 values[],
									size_t nValues) {
    return fmi3Error;
}

fmi3Status fmi3DoStep(fmi3Instance instance,
					  fmi3Float64 currentCommunicationPoint,
					  fmi3Float64 communicationStepSize,
					  fmi3Boolean noSetFMUStatePriorToCurrentPoint,
					  fmi3Boolean* eventEncountered,
					  fmi3Boolean* terminateSimulation,
					  fmi3Boolean* earlyReturn,
					  fmi3Float64* lastSuccessfulTime) {
    return fmi3Error;
}


fmi3Float64 AIn1 = 0.0;
fmi3Float64 AIn2 = 0.0;
fmi3Float64 AOut = 0.0;
fmi3Float64 BIn  = 0.0;
fmi3Float64 BOut = 0.0;

fmi3IntervalQualifier CountdownClockQualifier = fmi3NotYetKnown;
fmi3Float64 CountdownClockInterval = 0.0;
fmi3Boolean OutputClockTicked = fmi3ClockInactive;

void activateModelPartition10ms(ModelInstance *instance,
								fmi3Float64 activationTime ){
	
	fmi3Boolean conditionForCountdownClockMet = (AIn1>AIn2);								
    if (conditionForCountdownClockMet)
    {
      CountdownClockQualifier = fmi3NewInterval;
	  fmi3Boolean clocksTicked = fmi3True;
      // inform simulation algorithm that the countdown clock has ticked
      instance->callbackIntermediateUpdate(instance->instanceEnvironment,
												activationTime,
												clocksTicked,
												fmi3False,
												fmi3False,
												fmi3False,
												fmi3False,
												NULL,
												NULL);
   }
   
	fmi3Boolean conditionForOutputClockMet = (AIn2>42.0);;
    if (conditionForOutputClockMet)
   {
      // outputClock ticks
      OutputClockTicked = fmi3ClockActive;
      // inform simulation algorithm that output clock has ticked
      fmi3Boolean clocksTicked = fmi3True;
      instance->callbackIntermediateUpdate (instance->instanceEnvironment,
												activationTime,
												clocksTicked,
												fmi3False,
												fmi3False,
												fmi3False,
												fmi3False,
												NULL,
												NULL);
   }
   
   AOut = AIn1 + AIn2;

}

fmi3Status fmi3GetIntervalDecimal(fmi3Instance instance,
								  const fmi3ValueReference valueReferences[],
								  size_t nValueReferences,
								  fmi3Float64 interval[],
								  fmi3IntervalQualifier qualifier[],
								  size_t nValues) {		
    int i;
	for (i=0; i<nValues; i++)
	{
		if (valueReferences[i] == 6)
		{
			interval[i] = CountdownClockInterval;
			qualifier[i] = CountdownClockQualifier;
			CountdownClockQualifier = fmi3NotYetKnown;
		}
		else
		{
			qualifier[i] = fmi3NotYetKnown;
		}
	}
    return fmi3OK;
}

fmi3Status fmi3GetClock  (fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  fmi3Clock values[],
						  size_t nValues){
	int i;
	for (i=0; i<nValues; i++)
	{
		if (valueReferences[i] == 7)
		{
			values[i] = OutputClockTicked;
			OutputClockTicked = fmi3ClockInactive;
		}
		else
		{
			values[i] = fmi3ClockInactive;
		}
	}
	return fmi3OK;
}

void activateModelPartition50ms(ModelInstance *instance,
								fmi3Float64 activationTime ){
	BOut = 2.2*BIn;
									
}

void activateModelPartitionAperiodic    (ModelInstance *instance,
										fmi3Float64 activationTime ){
									
}

fmi3Status fmi3ActivateModelPartition(fmi3Instance instance,
									  fmi3ValueReference clockReference,
									  size_t clockElementIndex,
									  fmi3Float64 activationTime) {
	ModelInstance* modelInstance = (ModelInstance *) instance;
	switch (clockReference)
	   {
		  case 5:
			 // Input clock 10msClock
			 activateModelPartition10ms(instance, activationTime);
		  case 6:
			 // Input clock AperiodicClock
			 activateModelPartitionAperiodic(instance, activationTime);
		  case 8:
			 // Input clock 50msClock
			 activateModelPartition50ms(instance, activationTime);
	   }
    return fmi3Error;
}


fmi3Status fmi3GetFloat64(fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  fmi3Float64 values[],
						  size_t nValues) {
	int i;
	for (i=0; i<nValues; i++)
	{
		switch (valueReferences[0])
	    {
			case 2:
				values[i] = AOut;
			case 4:

				values[i] = BOut;		
	   }
	}
	return fmi3OK;
}
fmi3Status fmi3SetFloat64(fmi3Instance instance,
						  const fmi3ValueReference valueReferences[],
						  size_t nValueReferences,
						  const fmi3Float64 values[],
						  size_t nValues){
	int i;
	for (i=0; i<nValues; i++)
	{
		switch (valueReferences[i])
	    {
			case 0:
				AIn1 = values[i];
			case 1:
				AIn2 = values[i];
			case 3:
				BIn = values[i];
	   }
	}
	return fmi3OK;
}
