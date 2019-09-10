/* [TODO] Open points*/
// - What grade of detail is necessary?
//    - Configuration Mode necessary?
//    - Are status checks necessary? 
// - How many slaves?
// - Clocks: How many and which clock types?
// - Create Value References
// - While(): Get outputs and Set Inputs at beginning of while()?


#include <stdlib.h>
#include <stdio.h>

#include "fmi3PlatformTypes.h"
#include "fmi3FunctionTypes.h"
#include "fmi3Functions.h"

typedef enum { // Depending on Co-Simulation Master
  eventMode,
  stepMode,
  reconfigurationMode
} Modes;

//////////////////////////
// Define callback

// Global variables
fmi3IntermediateUpdateInfo s_intermediateUpdateInfo;

// Callbacks
void cb_logMessage(fmi3InstanceEnvironment instanceEnvironment, fmi3String instanceName, fmi3Status status, fmi3String category, fmi3String message) {
  puts(message);
}

void* cb_allocateMemory(fmi3InstanceEnvironment instanceEnvironment, size_t nobj, size_t size) {
  return calloc(nobj, size);
}

void cb_freeMemory(fmi3InstanceEnvironment instanceEnvironment, void* obj) {
  free(obj);
}

fmi3Status cb_intermediateUpdate(fmi3InstanceEnvironment instanceEnvironment, fmi3IntermediateUpdateInfo* intermediateUpdateInfo)
{
  // Save intermediateUpdateInfo for later use
  s_intermediateUpdateInfo = *intermediateUpdateInfo;
}



int main(int argc, char* argv[]) {

  fmi3Float64 startTime, stopTime, h, step, tStop, tend, t, tc, lastSuccessfulTime;
  fmi3Status status = fmi3OK;
  fmi3Boolean discard;
  fmi3Boolean terminateSimulation = fmi3False;

  Modes mode;
  fmi3Boolean earlyReturn;
  fmi3Boolean inferredClocksTickedS;

  fmi3CallbackFunctions callbacks;
  fmi3Instance s;
  const char *s_GUID = "{8c4e810f-3da3-4a00-8276-176fa3c9f000}"; // [TODO]: Fill


  printf("Running CoSimulation example... ");


  // tag::CoSimulation[]
  //////////////////////////
  // Initialization sub-phase

  fmi3EventInfo s_eventInfo;

  // Set callback functions
  callbacks.logMessage          = cb_logMessage;
  callbacks.allocateMemory      = cb_allocateMemory;
  callbacks.freeMemory          = cb_freeMemory;
  // Signal that early return is supported by master
  callbacks.intermediateUpdate  = cb_intermediateUpdate;
  callbacks.lockPreemption = NULL; // Preemption not active
  callbacks.unlockPreemption  = NULL; // Preemption not active


  // Instantiate slave
  // Create pointer to information for identifying the FMU in callbacks 
  callbacks.instanceEnvironment = NULL;

  //set Co-Simulation mode
  fmi3CoSimulationConfiguration coSimulationConfiguration;
  coSimulationConfiguration.intermediateVariableGet         = fmi3False;
  coSimulationConfiguration.intermediateInternalVariableGet = fmi3False;
  coSimulationConfiguration.intermediateVariableSet         = fmi3False;
  coSimulationConfiguration.coSimulationMode                = fmi3ModeHybridCoSimulation;


  // Instantiate slave
  s = fmi3Instantiate("Slave", fmi3CoSimulation, s_GUID, NULL, &callbacks, fmi3False, fmi3True, &coSimulationConfiguration); // [TODO]: fmuResourceLocation --> NULL?

  if (s == NULL) return EXIT_FAILURE; // error



  // Configuration Mode
  //fmi3EnterConfigurationMode(s);
  //fmi3ExitConfigurationMode(s);


  // Setup Experiment
  // Start and stop time 
  startTime = 0;
  stopTime = 10;
  // Communication constant step size
  h = 0.01;

  // Set all variable start values (of "ScalarVariable / <type> / start")
  //fmi3SetReal / Integer / Boolean / String(s, ...); // [TODO]: To be set.
  //fmi3SetClock(s, ...); // [TODO]: To be set.

  fmi3SetupExperiment(s, fmi3False, 0.0, startTime, fmi3True, stopTime);



  // Initialization Mode
  fmi3EnterInitializationMode(s);

  // Set the input values at time = startTime 
  //fmi3SetReal / Integer / Boolean / String(s, ...); // [TODO]: To be set.
  //fmi3SetClock(s, ...); // [TODO]: To be set.

  fmi3ExitInitializationMode(s);



  //////////////////////////
  // Simulation sub-phase
  tc = startTime; // Starting master time
  tStop = stopTime; // Stopping master time
  step = h; // Starting non-zero step size
  inferredClocksTickedS = fmi3False; // [TODO]: To be set.

  mode = eventMode; // [TODO]: @Masoud recheck: continuousTimeMode --> eventMode

  while (tc < tStop) {
    if (0 /* Inferred clock is active at current time tc */) { // [TODO]: To be set.
      /*Set possible active inferred clocks to true or to false*/
      if(mode == stepMode) { // [TODO]: @Masoud recheck: continuousTimeMode --> stepMode
        status = fmi3EnterEventMode(s);
        if ((status == fmi3Fatal) || (status == fmi3Error)) break;
        mode = eventMode; 
      };
      //fmi3SetClock(s, ...); // [TODO]: To be set.
      //fmi3SetIntervalDecimal(s, ...); /*Only needed if interval changes*/ // [TODO]: To be set.
      //fmi3SetIntervalFraction(s, ...); /*Only needed if interval changes*/ // [TODO]: To be set.
    };



    if (mode == eventMode) {
      status = fmi3NewDiscreteStates(s, &s_eventInfo);
      if ((status == fmi3Fatal) || (status == fmi3Error)) break;
    }



    if(mode == stepMode) { // [TODO]: @Masoud recheck: continuousTimeMode --> stepMode
      // Step Mode (default mode)
      tend = tc+step;
      t = tend*2;
      earlyReturn = fmi3False;
      status = fmi3DoStep(s, tc, step, fmi3False, &earlyReturn); // [TODO]: noSetFMUStatePriorToCurrentPoint == fmi3False?

      switch (status) {
        case fmi3Discard:
          fmi3GetDoStepDiscardedStatus(s, &discard, &lastSuccessfulTime);
          if (discard == fmi3True) printf("Slave s wants to terminate simulation.");
        case fmi3Error:
        case fmi3Fatal:
          terminateSimulation = fmi3True;
          break;
        default:
          break;
      }

      if (terminateSimulation) break;



      if(earlyReturn) {
        t = s_intermediateUpdateInfo.intermediateUpdateTime;
        /*rollback FMUs to earliest event time*/
        fmi3EnterEventMode(s);
        if ((status == fmi3Fatal) || (status == fmi3Error)) break;
        mode = eventMode;
        tc = t;
      }
      else{
        tc = tend;
      };
    };



    if(s_intermediateUpdateInfo.clocksTicked) {
      //fmi3GetClock(s, ...); // [TODO]: To be set.
      //fmi3GetIntervalDecimal(s, /*Intervals*/,...); // [TODO]: To be set.
      //fmi3GetIntervalFraction(s, /*Intervals*/,...); // [TODO]: To be set.
    };



    if((mode == eventMode) && (s_eventInfo.newDiscreteStatesNeeded == fmi3False) && (1 /*no clocks from GetClock() are active*/)) { // [TODO]: To be set.
      status = fmi3EnterStepMode(s); // [TODO]: @Masoud recheck: EnterContinuousTimeMode --> fmi3EnterStepMode
      if ((status == fmi3Fatal) || (status == fmi3Error)) break;
      mode = stepMode;
      //step = min(/*Intervals*/, s_eventInfo.nextEventTime); // [TODO]: To be set.
    };


    // Get outputs
    //fmi3GetReal/Integer/Boolean/String(s, ...); // [TODO]: To be set.
    // Set inputs
    //fmi3SetReal/Integer/Boolean/String(s, ...); // [TODO]: To be set.

  }


  //////////////////////////
  // Shutdown sub-phase
  if ((status != fmi3Error) && (status != fmi3Fatal)) {
    fmi3Terminate(s);
  }

  if (status != fmi3Fatal) {
    fmi3FreeInstance(s);
  }
  // end::CoSimulation[]

  printf("done.\n");

  return EXIT_SUCCESS;
}