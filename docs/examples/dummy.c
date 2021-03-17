#define FMI3_FUNCTION_PREFIX

#include "fmi3Functions.h"

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
fmi3CallbackLockPreemption lockPreemption;
fmi3CallbackUnlockPreemption unlockPreemption;

// tag::Example4[]
uint16_t DataBuffer[3]; // global buffer

void Task1(void) // low priority
{
    // ...
    // write data to DataBuffer
    lockPreemption();
    DataBuffer[0] = internal_out_RootSys1_1;
    DataBuffer[1] = internal_out_RootSys1_2;
    DataBuffer[2] = internal_out_RootSys1_3;
    unlockPreemption();
    // ...
 }

// ...

void Task2(void) // high priority
{
    // ...
    // read data from DataBuffer
    lockPreemption();
    internal_in_RootSys2_1 = DataBuffer[0];
    internal_in_RootSys2_2 = DataBuffer[1];
    internal_in_RootSys2_3 = DataBuffer[2];
    unlockPreemption();
    // ...
}
// end::Example4[]

// tag::Example5[]
void Task10ms_Execute()
{
//   // Set inputs with valueReference 0 and 1 associated to clockIndex 5
//   fmi3SetFloat64(s, {0,1}, &AIn);
//   // call for 10msClock tick (clockIndex 5)
//   fmi3ActivateModelPartition(s, 5, 0, Task10ms.ActivationTime);
//   // Get output with valueReference 2 associated to clockIndex 0
//   fmi3GetFloat64(s, {2}, &AOut);
};
// end::Example5[]

// tag::Example6[]
//void CallbackIntermediateUpdate(...,fmi3Boolean clocksTicked, ...)
//{
//   if (clocksTicked == fmi3True)
//   {
//      // ask FMU if countdown clock is about to tick
//      fmi3ValueReference aperiodicClockReferences = {6};
//      fmi3GetIntervalDecimal(... aperiodicClockReferences, ... &interval, &qualifier, ...);
//      if (qualifier[0] == fmi3NewInterval)
//      {
//         // schedule task for AperiodicClock with a delay
//         Scheduler->ScheduleTask(TaskAperiodic, interval[0]);
//      }
//
//      // ask FMU if output clock has ticked
//      fmi3ValueReference outputClockReferences = {7};
//      fmi3Boolean[] clocksActivationState = {fmi3ClockInactive};
//      fmi3GetClocks(... outputClockReferences, &clocksActivationState, ...);
//      if (clocksActivationState[0])
//      {
//         // schedule some external task
//         Scheduler->ScheduleTask(SomeTask);
//      }
//   }
//}
// end::Example6[]

// tag::Example7[]
//fmi3Status fmi3ActivateModelPartition(fmi3Instance *instance,
//   fmi3ValueReference clockReference, fmi3Float64 activationTime)
//{
//   switch (clockReference)
//   {
//      case 5:
//         // Input clock 10msClock
//         activateModelPartition10ms(instance, activationTime);
//      case 6:
//         // Input clock AperiodicClock
//         activateModelPartitionAperiodic(instance, activationTime);
//      case 8:
//         // Input clock 50msClock
//         activateModelPartition50ms(instance, activationTime);
//      ...
//   }
//}
// end::Example7[]

// tag::Example8[]
//void activateModelPartition10ms(fmi3Instance *instance, ...)
//{
//   ...
//   if (...)
//   {
//      CountdownClockQualifier = fmi3NewInterval;
//      CountdownClockInterval = 0.0;
//      // inform simulation algorithm that the countdown clock has ticked
//      fmi3Boolean clocksTicked = fmi3True;
//      instance->fmi3CallbackIntermediateUpdate(..., clocksTicked, ...);
//   }
//    if (...)
//   {
//      // outputClock ticks
//      fmi3SetClock({7});
//      // inform simulation algorithm that output clock has ticked
//      fmi3Boolean clocksTicked = fmi3True;
//      instance->fmi3CallbackIntermediateUpdate(..., clocksTicked, ...);
//   }
//   ...
//}
// end::Example8[]

// tag::Example9[]
//fmi3Status fmi3GetIntervalDecimal(..., fmi3ValueReference inputClockReferences[],
//   fmi3Float64 interval[],
//   fmi3IntervalQualifier qualifier[], ...)
//{
//   if (inputClockReferences[0] == 8)
//   {
//      interval[0] = CountdownClockInterval;
//      qualifier[0] = CountdownClockQualifier;
//      CountdownClockQualifier = fmi3NotYetKnown;
//   }
//}
// end::Example9[]

// tag::Example10[]
//fmi3Status fmi3GetClock(..., fmi3ValueReference outputClockReferences,
//   fmi3Clock *clocksActivationState, ...)
//{
//   if (outputClockReferences[0] == 7)
//   {
//      clocksActivationState[0] = OutClockActivationState;
//      OutClockActivationState = fmi3ClockInactive;
//   }
//}
// end::Example10[]


typedef struct {
    
    fmi3Instance instance;
    fmi3GetFloat64TYPE* fmi3GetFloat64;
    fmi3SetFloat64TYPE* fmi3SetFloat64;
    fmi3EnterEventModeTYPE* fmi3EnterEventMode;
    fmi3UpdateDiscreteStatesTYPE* fmi3UpdateDiscreteStates;
    fmi3EnterStepModeTYPE* fmi3EnterStepMode;
    fmi3EnterContinuousTimeModeTYPE* fmi3EnterContinuousTimeMode;
    bool discreteStatesNeedUpdate;

} FMU;

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

int main(int argc, char* argv[]) {
    return 0;
}
