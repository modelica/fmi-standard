#define FMI3_FUNCTION_PREFIX

#include "fmi3Functions.h"

/* tag::structuralParameterExample[] */
double A[3][2] = { {0.0, 0.1},
                   {1.0, 1.1},
                   {2.0, 2.1}};
/* end::structuralParameterExample[] */

typedef struct {
    fmi3Instance instance;
    fmi3InstantiateCoSimulationTYPE* fmi3InstantiateCoSimulation;
    fmi3EnterInitializationModeTYPE* fmi3EnterInitializationMode;
    fmi3ExitInitializationModeTYPE* fmi3ExitInitializationMode;
    fmi3DoStepTYPE* fmi3DoStep;
    fmi3TerminateTYPE* fmi3Terminate;
    fmi3FreeInstanceTYPE* fmi3FreeInstance;
    fmi3GetFloat64TYPE* fmi3GetFloat64;
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
} FMU;

typedef struct {

    fmi3LockPreemptionCallback lockPreemption;
    fmi3UnlockPreemptionCallback unlockPreemption;

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


typedef struct {

    fmi3ClockUpdateCallback callbackClockUpdate;
    fmi3LockPreemptionCallback lockPreemption;
    fmi3UnlockPreemptionCallback unlockPreemption;

    void* instanceEnvironment;

    // SE variables
    fmi3Float64 AIn1; // 0.0
    fmi3Float64 AIn2; // 0.0
    fmi3Float64 AOut; // 0.0
    fmi3Float64 BIn; // 0.0
    fmi3Float64 BOut; // 0.0

    fmi3IntervalQualifier CountdownClockQualifier; // fmi3IntervalNotYetKnown
    fmi3Float64 CountdownClockInterval; // 0.0
    fmi3Boolean OutputClockTicked; // fmi3ClockInactive

} ModelInstance;


fmi3ValueReference AInputReferences[] = {0, 1};
fmi3ValueReference AOutputReferences[] = {2};
fmi3Float64 AInput[] = {0.0, 0.0};
fmi3Float64 AOutput[] = {0.0};
fmi3ValueReference ClockReference10ms = 5;

fmi3Float64 ActivationTime = 0.0;

Environment *env;

fmi3Instance fmu;


// SE functions (dummies)
void ScheduleExternalTask() {}
void ScheduleAperiodicTask(fmi3Float64 interval) {}
void activateModelPartition50ms(fmi3Instance* instance, fmi3Float64 activationTime) {
    ModelInstance * inst = (ModelInstance *) instance;
    inst->BOut = 2.2 * inst->BIn;
}

/* tag::SE_sa_task10ms[] */
void ExecuteModelPartition10ms() {
    fmi3SetFloat64(fmu, AInputReferences, 2, AInput, 2);
    fmi3ActivateModelPartition(fmu, ClockReference10ms, ActivationTime);
    fmi3GetFloat64(fmu, AOutputReferences, 1, AOutput, 1);
}
/* end::SE_sa_task10ms[] */

/* tag::SE_sa_clockUpdate[] */
void CallbackClockUpdate(fmi3InstanceEnvironment instanceEnvironment) {
    fmi3Float64 interval[] = { 0.0 };
    fmi3IntervalQualifier intervalQualifier[] = { fmi3IntervalNotYetKnown };

    // ask FMU if countdown clock AperiodicClock is about to tick
    const fmi3ValueReference aperiodicClockReferences[] = { 6 };
    fmi3GetIntervalDecimal(fmu, aperiodicClockReferences, 1, interval, intervalQualifier);
    if (intervalQualifier[0] == fmi3IntervalChanged) {
        // schedule task for AperiodicClock with a delay
        ScheduleAperiodicTask(interval[0]);
    }
    // ask FMU if OutputClock has ticked
    fmi3ValueReference outputClockReferences[] = { 7 };
    fmi3Boolean clockActivationStates[] = { fmi3ClockInactive };
    fmi3GetClock(fmu, outputClockReferences, 1, clockActivationStates);
    if (clockActivationStates[0]) {
        // schedule some external task
        ScheduleExternalTask();
    }
}
/* end::SE_sa_clockUpdate[] */

/* tag::SE_fmu_activateMP10ms[] */
void activateModelPartition10ms(fmi3Instance instance, fmi3Float64 activationTime) {
    ModelInstance * inst = (ModelInstance *) instance;

    fmi3Boolean conditionForCountdownClockMet = (inst->AIn1 > inst->AIn2);
    if (conditionForCountdownClockMet) {
        inst->CountdownClockQualifier = fmi3IntervalChanged;
        inst->CountdownClockInterval = 0.0;
        // inform the simulation algorithm that countdown clock AperiodicClock is about to tick
        inst->callbackClockUpdate(inst->instanceEnvironment);
    }

    fmi3Boolean conditionForOutputClockMet = (inst->AIn2 > 42.0);
    if (conditionForOutputClockMet) {
        // OutputClock ticks
        inst->OutputClockTicked = fmi3ClockActive;
        // inform the simulation algorithm that OutputClock has ticked
        inst->callbackClockUpdate(inst->instanceEnvironment);
    }
    inst->AOut = inst->AIn1 + inst->AIn2;
}
/* end::SE_fmu_activateMP10ms[] */

/* tag::SE_fmu_activateMPAperiodic[] */
void activateModelPartitionAperiodic(fmi3Instance instance, fmi3Float64 activationTime) {
    ModelInstance* inst = (ModelInstance*)instance;
    inst->CountdownClockQualifier = fmi3IntervalNotYetKnown;
    // ...
}
/* end::SE_fmu_activateMPAperiodic[] */

/* tag::SE_fmu_activateMP[] */
fmi3Status fmi3ActivateModelPartition(fmi3Instance instance,
                                      fmi3ValueReference clockReference,
                                      fmi3Float64 activationTime) {
    switch (clockReference) {
    case 5:    // Input clock 10msClock
        activateModelPartition10ms(instance, activationTime);
        break;
    case 6:    // Input clock AperiodicClock
        activateModelPartitionAperiodic(instance, activationTime);
        break;
    case 8:    // Input clock 50msClock
        activateModelPartition50ms(instance, activationTime);
        break;
    // ...
    }
    return fmi3OK;
}
/* end::SE_fmu_activateMP[] */

/* tag::SE_fmu_getIntervalDecimal[] */
fmi3Status fmi3GetIntervalDecimal(fmi3Instance instance,
                                  const fmi3ValueReference valueReferences[],
                                  size_t nValueReferences,
                                  fmi3Float64 interval[],
                                  fmi3IntervalQualifier qualifier[]) {
    ModelInstance * inst = (ModelInstance *) instance;

    for (int i = 0; i < nValueReferences; i++) {
        switch (valueReferences[i]) {
        case 5:    // Input clock 10msClock
            interval[i] = 0.01;
            qualifier[i] = fmi3IntervalUnchanged;
            break;
        case 6:    // Input clock AperiodicClock
            env->lockPreemption(); // Optional: Preventing preemption is actually not needed here.
            interval[i] = inst->CountdownClockInterval;
            qualifier[i] = inst->CountdownClockQualifier;
            if (inst->CountdownClockQualifier == fmi3IntervalChanged) {
                inst->CountdownClockQualifier = fmi3IntervalUnchanged;
            }
            env->unlockPreemption();
            break;
        case 8:    // Input clock 50msClock
            interval[i] = 0.05;
            qualifier[i] = fmi3IntervalUnchanged;
        // ...
        }
    }
    return fmi3OK;
}
/* end::SE_fmu_getIntervalDecimal[] */

/* tag::SE_fmu_getClock[] */
fmi3Status fmi3GetClock(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        fmi3Clock values[]) {
    ModelInstance * inst = (ModelInstance *) instance;

    for (int i = 0; i < nValueReferences; i++) {
        switch (valueReferences[i]) {
        case 7:    // OutputClock
            env->lockPreemption(); // Optional: Preventing preemption is actually not needed here.
            values[i] = inst->OutputClockTicked;
            inst->OutputClockTicked = fmi3ClockInactive;
            env->unlockPreemption();
            break;
        // ...
        }
    }
    return fmi3OK;
}
/* end::SE_fmu_getClock[] */

void algebraicLoop1() {

    // tag::AlgebraicLoop1[]
    FMU *M1, *M2;
    fmi3ValueReference vr_M1_u, vr_M1_y, vr_M2_u1, vr_M2_u2, vr_M2_y1;
    fmi3Float64 s = 0.1, M2_y1, M1_y, M2_y2;
    // ...
    M2->fmi3SetFloat64(M2->instance, &vr_M2_u1, 1, &s, 1);
    M2->fmi3GetFloat64(M2->instance, &vr_M2_y1, 1, &M2_y1, 1);
    M1->fmi3SetFloat64(M1->instance, &vr_M1_u,  1, &M2_y1, 1);
    M1->fmi3GetFloat64(M1->instance, &vr_M1_y,  1, &M1_y, 1);
    M2->fmi3SetFloat64(M2->instance, &vr_M2_u2, 1, &M1_y, 1);
    M2->fmi3GetFloat64(M2->instance, &vr_M2_y1, 1, &M2_y2, 1);
    //...
    // end::AlgebraicLoop1[]
}

void algebraicLoop2() {
    // tag::AlgebraicLoop2[]
    FMU *M3, *M4;
    fmi3ValueReference vr_M3_u, vr_M3_y, vr_M4_u, vr_M4_y;
    fmi3Float64 s, M3_y, M4_y, residual, tolerance;
    bool converged = false;

    while (!converged) { // start iteration
        // s determined by the solver
        // ...
        M4->fmi3SetFloat64(M4->instance, &vr_M4_u, 1, &s, 1);
        M4->fmi3GetFloat64(M4->instance, &vr_M4_y, 1, &M4_y, 1);
        M3->fmi3SetFloat64(M3->instance, &vr_M3_u, 1, &M4_y, 1);
        M3->fmi3GetFloat64(M3->instance, &vr_M3_y, 1, &M3_y, 1);
        residual = s - M3_y; // provided to the solver
        converged = residual < tolerance;
    }
    // end::AlgebraicLoop2[]
}

void algebraicLoop3() {

    fmi3Boolean M1_DStatesNeedUpdate, M2_DStatesNeedUpdate, *p2, *p3, *p4, *p5;
    fmi3Float64 *p6;
    bool isCoSimulation;

    // tag::AlgebraicLoop3[]
    FMU *M1, *M2;  // structures that hold the functions and instances of the FMUs

    M1->fmi3EnterEventMode(M1->instance);
    M2->fmi3EnterEventMode(M2->instance);

    // start event iteration
    do {
        // solve algebraic loop as described in the sample codes above

        // introduce new instant of super-dense time
        M1->fmi3UpdateDiscreteStates(M1->instance, &M1_DStatesNeedUpdate, p2, p3, p4, p5, p6);
        M2->fmi3UpdateDiscreteStates(M2->instance, &M2_DStatesNeedUpdate, p2, p3, p4, p5, p6);

    } while (M1_DStatesNeedUpdate || M2_DStatesNeedUpdate);

    if (isCoSimulation) {
        // Co-Simulation
        M1->fmi3EnterStepMode(M1->instance);
        M2->fmi3EnterStepMode(M2->instance);
    } else {
        // Model Exchange
        M1->fmi3EnterContinuousTimeMode(M1->instance);
        M2->fmi3EnterContinuousTimeMode(M2->instance);
    }
    // end::AlgebraicLoop3[]
}

int connectedFMUs() {

    fmi3Float64 startTime, stopTime, h, time;
    int nSteps;
    fmi3Status status = fmi3OK;
    const char *guid;
    FMU* M1;
    FMU* M2;
    fmi3Instance s1, s2;
    fmi3Boolean eventEncountered, terminateSimulation, earlyReturn;
    fmi3Float64 lastSuccessfulTime;
    fmi3LogMessageCallback cb_logMessage;

    // tag::ConnectedFMUs[]
    ////////////////////////////
    // Initialization sub-phase

    // instantiate both FMUs
    s1 = M1->fmi3InstantiateCoSimulation("s1",          // instanceName
                                         guid,          // instantiationToken
                                         NULL,          // resourceLocation
                                         fmi3False,     // visible
                                         fmi3False,     // loggingOn
                                         fmi3False,     // eventModeUsed
                                         fmi3False,     // earlyReturnAllowed
                                         NULL,          // requiredIntermediateVariables
                                         0,             // nRequiredIntermediateVariables
                                         NULL,          // instanceEnvironment
                                         cb_logMessage, // logMessage
                                         NULL);         // intermediateUpdate

    s2 = M2->fmi3InstantiateCoSimulation("s2",          // instanceName
                                         guid,          // instantiationToken
                                         NULL,          // resourceLocation
                                         fmi3False,     // visible
                                         fmi3False,     // loggingOn
                                         fmi3False,     // eventModeUsed
                                         fmi3False,     // earlyReturnAllowed
                                         NULL,          // requiredIntermediateVariables
                                         0,             // nRequiredIntermediateVariables
                                         NULL,          // instanceEnvironment
                                         cb_logMessage, // logMessage
                                         NULL);         // intermediateUpdate

    if (s1 == NULL || s2 == NULL)
        return EXIT_FAILURE;

    // start and stop time
    startTime = 0;
    stopTime = 10;

    // communication step size
    h = 0.01;

    // set all variable start values (of "ScalarVariable / <type> / start")
    // s1_fmi3Set{VariableType}(s1, ...);
    // s2_fmi3Set{VariableType}(s2, ...);

    // initialize the FMUs
    M1->fmi3EnterInitializationMode(s1, fmi3False, 0.0, startTime, fmi3True, stopTime);
    M2->fmi3EnterInitializationMode(s2, fmi3False, 0.0, startTime, fmi3True, stopTime);

    // set the input values at time = startTime
    // fmi3Set{VariableType}(s1, ...);
    // fmi3Set{VariableType}(s2, ...);

    M1->fmi3ExitInitializationMode(s1);
    M2->fmi3ExitInitializationMode(s2);

    ////////////////////////
    // Simulation sub-phase
    time = startTime; // current time

    nSteps = 0;

    while ((time < stopTime) && (status == fmi3OK)) {

        // retrieve outputs
        // fmi3Get{VariableType}(s1, ..., 1, &y1);
        // fmi3Get{VariableType}(s2, ..., 1, &y2);

        // set inputs
        // fmi3Set{VariableType}(s1, ..., 1, &y2);
        // fmi3Set{VariableType}(s2, ..., 1, &y1);

        // call instance s1 and check status
        status = M1->fmi3DoStep(s1, time, h, fmi3True, &eventEncountered, &terminateSimulation, &earlyReturn, &lastSuccessfulTime);

        if (terminateSimulation) {
            break;  // Instance s1 requested to terminate simulation.
        }

        // call instance s2 and check status as above
        status = M2->fmi3DoStep(s2, time, h, fmi3True, &eventEncountered, &terminateSimulation, &earlyReturn, &lastSuccessfulTime);

        // ...

        // increment current time
        time = (++nSteps) * h;
     }

    //////////////////////////
    // Shutdown sub-phase
    if (status != fmi3Error && status != fmi3Fatal) {
        M1->fmi3Terminate(s1);
        M2->fmi3Terminate(s2);
    }

    if (status != fmi3Fatal) {
        M1->fmi3FreeInstance(s1);
        M2->fmi3FreeInstance(s2);
    }

    return EXIT_SUCCESS;
    // end::ConnectedFMUs[]
}

// dummy main function
int main(int argc, char* argv[]) {
    return 0;
}
