#define FMI3_FUNCTION_PREFIX

#include "fmi3Functions.h"

// tag::ModelStruct1[]
typedef struct {
    
    fmi3Instance instance;
    fmi3GetFloat64TYPE* fmi3GetFloat64;
// end::ModelStruct1[]
    fmi3SetFloat64TYPE* fmi3SetFloat64;
    fmi3EnterEventModeTYPE* fmi3EnterEventMode;
    fmi3UpdateDiscreteStatesTYPE* fmi3UpdateDiscreteStates;
    fmi3EnterStepModeTYPE* fmi3EnterStepMode;
    fmi3EnterContinuousTimeModeTYPE* fmi3EnterContinuousTimeMode;
    fmi3ActivateModelPartitionTYPE* fmi3ActivateModelPartition;
    fmi3GetIntervalDecimalTYPE* fmi3GetIntervalDecimal;
    fmi3GetClockTYPE* fmi3GetClock;
    fmi3SetClockTYPE* fmi3SetClock;
    bool discreteStatesNeedUpdate;
// tag::ModelStruct2[]
} FMU;
// end::ModelStruct2[]

typedef struct {
    
    fmi3CallbackLockPreemption lockPreemption;
    fmi3CallbackUnlockPreemption unlockPreemption;

} Environment;

fmi3Status fmi3GetFloat64(fmi3Instance instance,
                          const fmi3ValueReference valueReferences[],
                          size_t nValueReferences,
                          fmi3Float64 values[],
                          size_t nValues) {
    return fmi3Error;
}

fmi3Status fmi3SetFloat64(fmi3Instance instance,
                          const fmi3ValueReference valueReferences[],
                          size_t nValueReferences,
                          const fmi3Float64 values[],
                          size_t nValues) {
    return fmi3Error;
}

uint16_t internal_out_RootSys1_1, internal_out_RootSys1_2, internal_out_RootSys1_3;
uint16_t internal_in_RootSys2_1, internal_in_RootSys2_2, internal_in_RootSys2_3;

// tag::Example4[]
Environment *env;
uint16_t DataBuffer[3]; // global buffer

void Task1(void) { // low priority

    // ...
    // write data to DataBuffer
    env->lockPreemption();
    DataBuffer[0] = internal_out_RootSys1_1;
    DataBuffer[1] = internal_out_RootSys1_2;
    DataBuffer[2] = internal_out_RootSys1_3;
    env->unlockPreemption();
    // ...
 }

// ...

void Task2(void) { // high priority

    // ...
    // read data from DataBuffer
    env->lockPreemption();
    internal_in_RootSys2_1 = DataBuffer[0];
    internal_in_RootSys2_2 = DataBuffer[1];
    internal_in_RootSys2_3 = DataBuffer[2];
    env->unlockPreemption();
    // ...
}
// end::Example4[]

// SE variables
fmi3Float64 AIn1 = 0.0;
fmi3Float64 AIn2 = 0.0;
fmi3Float64 AOut = 0.0;
fmi3Float64 BIn = 0.0;
fmi3Float64 BOut = 0.0;
fmi3IntervalQualifier CountdownClockQualifier = fmi3IntervalNotYetKnown;
fmi3Float64 CountdownClockInterval = 0.0;
fmi3Boolean OutputClockTicked = fmi3ClockInactive;
fmi3Float64 ActivationTime = 0.0;
fmi3ValueReference AInputReferences[] = { 0,1 };
fmi3ValueReference AOutputReferences[] = { 2 };
fmi3Float64 AInput[] = { 0.0, 0.0 };
fmi3Float64 AOutput[] = { 0.0 };
fmi3ValueReference ClockReference10ms = 5;
size_t ClockElementIndex = 0;

typedef struct {

	double time;
	const char* instanceName;
	const char* resourceLocation;

	fmi3CallbackLogMessage logger;
	fmi3CallbackIntermediateUpdate callbackIntermediateUpdate;
	fmi3CallbackLockPreemption lockPreemption;
	fmi3CallbackUnlockPreemption unlockPreemption;

	void* instanceEnvironment;
	bool clocksTicked;

	//ModelData *modelData;

} ModelInstance;
fmi3Instance fmu;


// SE functions (dummies)
void ScheduleExternalTask() {}
void ScheduleAperiodicTask(fmi3Float64 interval) {}
void activateModelPartition50ms(fmi3Instance* instance, fmi3Float64 activationTime) { BOut = 2.2 * BIn; }
void activateModelPartitionAperiodic(fmi3Instance* instance, fmi3Float64 activationTime) {}

/*tag::SE_sa_task10ms[] */
void ExecuteModelPartition10ms() {
	fmi3SetFloat64(fmu, AInputReferences, 2, AInput, 2);
	fmi3ActivateModelPartition(fmu, ClockReference10ms, ClockElementIndex, ActivationTime);
	fmi3GetFloat64(fmu, AOutputReferences, 1, AOutput, 1);
}
/* end::SE_sa_task10ms[] */

/* tag::SE_sa_intermediateUpdate[] */
void CallbackIntermediateUpdate(fmi3InstanceEnvironment instanceEnvironment,
	fmi3Float64  intermediateUpdateTime, fmi3Boolean  clocksTicked,
	fmi3Boolean  intermediateVariableSetRequested, fmi3Boolean  intermediateVariableGetAllowed,
	fmi3Boolean  intermediateStepFinished, fmi3Boolean  canReturnEarly,
	fmi3Boolean* earlyReturnRequested, fmi3Float64* earlyReturnTime) {

	fmi3Float64 interval[] = { 0.0 };
	fmi3IntervalQualifier intervalQualifier[] = { fmi3IntervalNotYetKnown };
	if (clocksTicked == fmi3True) {
		// ask FMU if countdown clock is about to tick
		const fmi3ValueReference aperiodicClockReferences[] = { 6 };
		fmi3GetIntervalDecimal(fmu, aperiodicClockReferences, 1, interval, intervalQualifier, 1);
		if (intervalQualifier[0] == fmi3IntervalChanged) {
			// schedule task for AperiodicClock with a delay
			ScheduleAperiodicTask(interval[0]);
		}
		// ask FMU if output clock has ticked
		fmi3ValueReference outputClockReferences[] = { 7 };
		fmi3Boolean clocksActivationState[] = { fmi3ClockInactive };
		fmi3GetClock(fmu, outputClockReferences, 1, clocksActivationState, 1);
		if (clocksActivationState[0]) {
			// schedule some external task
			ScheduleExternalTask();
		}
	}
}
/* end::SE_sa_intermediateUpdate[] */

/* tag::SE_fmu_activateMP10ms[] */
void activateModelPartition10ms(ModelInstance* instance, fmi3Float64 activationTime) {

	fmi3Boolean conditionForCountdownClockMet = (AIn1 > AIn2);
	if (conditionForCountdownClockMet) {
		CountdownClockQualifier = fmi3IntervalChanged;
		CountdownClockInterval = 0.0;
		// inform simulation algorithm that the countdown clock has ticked
		fmi3Boolean clocksTicked = fmi3True;
		instance->callbackIntermediateUpdate(instance->instanceEnvironment, activationTime, clocksTicked,
			fmi3False, fmi3False, fmi3False, fmi3False, NULL, NULL);
	}
	fmi3Boolean conditionForOutputClockMet = (AIn2 > 42.0);
	if (conditionForOutputClockMet) {
		// outputClock ticks
		OutputClockTicked = fmi3ClockActive;
		// inform simulation algorithm that output clock has ticked
		fmi3Boolean clocksTicked = fmi3True;
		instance->callbackIntermediateUpdate(instance->instanceEnvironment, activationTime, clocksTicked,
			fmi3False, fmi3False, fmi3False, fmi3False, NULL, NULL);
	}
	AOut = AIn1 + AIn2;
}
/* end::SE_fmu_activateMP10ms[] */

/* tag::SE_fmu_activateMP[] */
fmi3Status fmi3ActivateModelPartition(fmi3Instance instance, fmi3ValueReference clockReference,
	size_t clockElementIndex, fmi3Float64 activationTime) {

	switch (clockReference) {
	case 5:
		// Input clock 10msClock
		activateModelPartition10ms(instance, activationTime);
		break;
	case 6:
		// Input clock AperiodicClock
		activateModelPartitionAperiodic(instance, activationTime);
		break;
	case 8:
		// Input clock 50msClock
		activateModelPartition50ms(instance, activationTime);
		break;
		// ...
	}
	return fmi3OK;
}
/* end::SE_fmu_activateMP[] */

/* tag::SE_fmu_getIntervalDecimal[] */
fmi3Status fmi3GetIntervalDecimal(fmi3Instance instance, const fmi3ValueReference valueReferences[],
	size_t nValueReferences, fmi3Float64 interval[], fmi3IntervalQualifier qualifier[], size_t nValues) {

	for (int i = 0; i < nValues; i++) {
		if (valueReferences[i] == 6) {
            env->lockPreemption(); // Optional: Preventing preemption is actually not needed here.
			interval[i] = CountdownClockInterval;
			qualifier[i] = CountdownClockQualifier;
			CountdownClockQualifier = fmi3IntervalUnchanged;
            env->unlockPreemption();
		}
		else {
			qualifier[i] = fmi3IntervalNotYetKnown;
		}
	}
	return fmi3OK;
}
/* end::SE_fmu_getIntervalDecimal[] */

/* tag::SE_fmu_getClock[] */
fmi3Status fmi3GetClock(fmi3Instance instance, const fmi3ValueReference valueReferences[],
	size_t nValueReferences, fmi3Clock values[], size_t nValues) {

	for (int i = 0; i < nValues; i++) {
		if (valueReferences[i] == 7) {
            env->lockPreemption(); // Optional: Preventing preemption is actually not needed here.
			values[i] = OutputClockTicked;
			OutputClockTicked = fmi3ClockInactive;
            env->unlockPreemption();
		}
		else {
			values[i] = fmi3ClockInactive;
		}
	}
	return fmi3OK;
}
/* end::SE_fmu_getClock[] */

void algebraicLoop1() {
    
    // tag::AlgebraicLoop1[]
    FMU *m1, *m2;
    fmi3ValueReference vr_M1_u, vr_M1_y, vr_M2_u1, vr_M2_u2, vr_M2_y1, vr_M2_y2;
    fmi3Float64 s = 0.1, M2_y1, M1_y, M2_y2;
    // ...
    m2->fmi3SetFloat64(m2->instance, &vr_M2_u1, 1, &s, 1);
    m2->fmi3GetFloat64(m2->instance, &vr_M2_y1, 1, &M2_y1, 1);
    m1->fmi3SetFloat64(m1->instance, &vr_M1_u,  1, &M2_y1, 1);
    m1->fmi3GetFloat64(m1->instance, &vr_M1_y,  1, &M1_y, 1);
    m2->fmi3SetFloat64(m2->instance, &vr_M2_u2, 1, &M1_y, 1);
    m2->fmi3GetFloat64(m2->instance, &vr_M2_y1, 1, &M2_y2, 1);
    //...
    // end::AlgebraicLoop1[]
}

void algebraicLoop2() {
    // tag::AlgebraicLoop2[]
    FMU *m3, *m4;
    fmi3ValueReference vr_M3_u, vr_M3_y, vr_M4_u, vr_M4_y;
    fmi3Float64 s, M3_y, M4_y, residual, tolerance;
    bool converged = false;

    while (!converged) { // start iteration
        // s determined by the solver
        // ...
        m4->fmi3SetFloat64(m4->instance, &vr_M4_u, 1, &s, 1);
        m4->fmi3GetFloat64(m4->instance, &vr_M4_y, 1, &M4_y, 1);
        m3->fmi3SetFloat64(m3->instance, &vr_M3_u, 1, &M4_y, 1);
        m3->fmi3GetFloat64(m3->instance, &vr_M3_y, 1, &M3_y, 1);
        residual = s - M3_y; // provided to the solver
        converged = residual < tolerance;
    }
    // end::AlgebraicLoop2[]
}

void algebraicLoop3() {

    fmi3Boolean v1, v2, v5;
    fmi3Int32 *v3;
    size_t v4;
    
    fmi3Boolean m1_DStatesNeedUpdate, m2_DStatesNeedUpdate, *p2, *p3, *p4, *p5;
    fmi3Float64 *p6;
    bool isCoSimulation;
    
    // tag::AlgebraicLoop3[]
    FMU *m1, *m2;  // structures that hold the functions and instances of the FMUs
    
    m1->fmi3EnterEventMode(m1->instance, v1, v2, v3, v4, v5);
    m2->fmi3EnterEventMode(m2->instance, v1, v2, v3, v4, v5);
    
    // start event iteration
    do {
        // solve algebraic loop as described in the sample codes above

        // introduce new instant of super-dense time
        m1->fmi3UpdateDiscreteStates(m1->instance, &m1_DStatesNeedUpdate, p2, p3, p4, p5, p6);
        m2->fmi3UpdateDiscreteStates(m2->instance, &m2_DStatesNeedUpdate, p2, p3, p4, p5, p6);
        
    } while (m1_DStatesNeedUpdate || m2_DStatesNeedUpdate);

    if (isCoSimulation) {
        // Co-Simulation
        m1->fmi3EnterStepMode(m1->instance);
        m2->fmi3EnterStepMode(m2->instance);
    } else {
        // Model Exchange
        m1->fmi3EnterContinuousTimeMode(m1->instance);
        m2->fmi3EnterContinuousTimeMode(m2->instance);
    }
    // end::AlgebraicLoop3[]
}

// dummy main function
int main(int argc, char* argv[]) {
    return 0;
}
