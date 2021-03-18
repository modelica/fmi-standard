#define FMI3_FUNCTION_PREFIX

#include "fmi3Functions.h"

typedef struct {
    
    fmi3Instance instance;
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

// tag::Example5[]
void Task10ms_Execute() {

    FMU *m; // FMU instance
    fmi3ValueReference vr_in[2] = {0, 1}, vr_out[1] = {2};
    fmi3Float64 AIn[2], AOut[1], Task10ms_ActivationTime;

    // Set inputs with valueReference 0 and 1 associated to clockIndex 5
    m->fmi3SetFloat64(m->instance, vr_in, 2, AIn, 2);

    // call for 10msClock tick (clockIndex 5)
    m->fmi3ActivateModelPartition(m->instance, 5, 0, Task10ms_ActivationTime);

    // Get output with valueReference 2 associated to clockIndex 0
    m->fmi3GetFloat64(m->instance, vr_out, 1, AOut, 1);
};
// end::Example5[]

// tag::Example6[]
void CallbackIntermediateUpdate(/*..., */ fmi3Boolean clocksTicked /*, ...*/) {

    FMU *m;
    
    if (clocksTicked) {
        
        // ask FMU if countdown clock is about to tick
        fmi3ValueReference aperiodicClockReferences[1] = { 6 };
        fmi3Float64 intervals[1];
        fmi3IntervalQualifier qualifiers[1];
        
        m->fmi3GetIntervalDecimal(m->instance, aperiodicClockReferences, 1, intervals, qualifiers, 1);
        
        if (qualifiers[0] == fmi3NewInterval) {
            // schedule task for AperiodicClock with a delay
            // Scheduler->ScheduleTask(TaskAperiodic, intervals[0]);
        }

        // ask FMU if output clock has ticked
        fmi3ValueReference outputClockReferences = { 7 };
        fmi3Boolean clocksActivationState[1] = { fmi3ClockInactive };
        
        m->fmi3GetClock(m->instance, outputClockReferences, 1, clocksActivationState, 1);
        
        if (clocksActivationState[0]) {
            // schedule some external task
            // Scheduler->ScheduleTask(SomeTask);
        }
    }
}
// end::Example6[]

void activateModelPartition10ms(fmi3Instance instance, fmi3Float64 activationTime);
void activateModelPartitionAperiodic(fmi3Instance instance, fmi3Float64 activationTime) {}
void activateModelPartition50ms(fmi3Instance instance, fmi3Float64 activationTime) {}

// tag::Example7[]
fmi3Status fmi3ActivateModelPartition(fmi3Instance instance,
                                      fmi3ValueReference clockReference,
                                      size_t clockElementIndex,
                                      fmi3Float64 activationTime) {
    
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
}
// end::Example7[]

bool condition1, condition2;
fmi3IntervalQualifier CountdownClockQualifier;
fmi3Float64 CountdownClockInterval;

// tag::Example8[]
void activateModelPartition10ms(fmi3Instance instance, fmi3Float64 activationTime) {
    
    // ...
    
    FMU *m;
    
    if (condition1) {
        CountdownClockQualifier = fmi3NewInterval;
        CountdownClockInterval = 0.0;
        // inform simulation algorithm that the countdown clock has ticked
        fmi3Boolean clocksTicked = fmi3True;
        // fmi3CallbackIntermediateUpdate(..., clocksTicked, ...);
    }
    
    if (condition2) {
        // outputClock ticks
        // fmi3SetClock({7});
        // inform simulation algorithm that output clock has ticked
        // fmi3Boolean clocksTicked = fmi3True;
        // instance->fmi3CallbackIntermediateUpdate(..., clocksTicked, ...);
    }
    
    // ...
}
// end::Example8[]

fmi3Float64 CountdownClockInterval;
fmi3IntervalQualifier CountdownClockQualifier;

// tag::Example9[]
fmi3Status fmi3GetIntervalDecimal(fmi3Instance instance,
                                  const fmi3ValueReference valueReferences[],
                                  size_t nValueReferences,
                                  fmi3Float64 intervals[],
                                  fmi3IntervalQualifier qualifiers[],
                                  size_t nIntervals) {
   
    if (valueReferences[0] == 8) {
        intervals[0] = CountdownClockInterval;
        qualifiers[0] = CountdownClockQualifier;
        CountdownClockQualifier = fmi3NotYetKnown;
    }
}
// end::Example9[]

fmi3Clock OutClockActivationState;

// tag::Example10[]
fmi3Status fmi3GetClock(fmi3Instance instance,
                        const fmi3ValueReference valueReferences[],
                        size_t nValueReferences,
                        fmi3Clock values[],
                        size_t nValues) {
    
    if (valueReferences[0] == 7) {
        values[0] = OutClockActivationState;
        OutClockActivationState = fmi3ClockInactive;
    }
}
// end::Example10[]


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
