#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

#define FMI3_FUNCTION_PREFIX M_
#include "fmi3Functions.h"
#undef FMI3_FUNCTION_PREFIX

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

void cb_logMessage(fmi3InstanceEnvironment instanceEnvironment, fmi3String instanceName, fmi3Status status, fmi3String category, fmi3String message) {
    puts(message);
}

void* cb_allocateMemory(fmi3InstanceEnvironment instanceEnvironment, size_t nobj, size_t size) {
    return calloc(nobj, size);
}

void  cb_freeMemory(fmi3InstanceEnvironment instanceEnvironment, void* obj)  {
    free(obj);
}


int main(int argc, char* argv[]) {

    fmi3Float64 h, tNext, tEnd = 10, time, tStart = 0;
    fmi3Boolean timeEvent, stateEvent, enterEventMode, terminateSimulation = fmi3False, initialEventMode, valuesOfContinuousStatesChanged, nominalsOfContinuousStatesChanged;
    fmi3EventInfo eventInfo;

    const char *guid = "{8c4e810f-3da3-4a00-8276-176fa3c9f000}";

    fmi3CallbackFunctions callbacks;
    fmi3Instance m;
    fmi3Float64 x[2], x_nominal[2], der_x[2], dt = 0.01, z[1], previous_z[1];
    size_t nx = 2, nz = 0;

    printf("Running model_exchange example... ");

    // tag::CoSimulation[]
    m = M_fmi3Instantiate("m", fmi3CoSimulation, guid, NULL, &callbacks, fmi3False, fmi3False);
    // "m" is the instance name
    // "M_" is the MODEL_IDENTIFIER

    // set the start time
    time  = tStart;
    
    // set all variable start values (of "ScalarVariable / <type> / start") and
    // set the start values at time = Tstart
    // M_fmi3SetReal/Integer/Boolean/String(m, ...)

    // initialize
    // determine continuous and discrete states
    M_fmi3SetupExperiment(m, fmi3False, 0.0, tStart, fmi3True, tEnd);
    M_fmi3EnterInitializationMode(m);
    M_fmi3ExitInitializationMode(m);
    
    initialEventMode = fmi3True;
    enterEventMode   = fmi3False;
    timeEvent        = fmi3False;
    stateEvent       = fmi3False;
    // previous_z = zeros(nz);
    
    // retrieve initial state x and
    // nominal values of x (if absolute tolerance is needed)
    M_fmi3GetContinuousStates(m, x, nx);
    M_fmi3GetNominalsOfContinuousStates(m, x_nominal, nx);
    
    // retrieve solution at t=Tstart, for example, for outputs
    // M_fmi2GetReal/Integer/Boolean/String(m, ...)
    
    while (!terminateSimulation) {
        
        // handle events
        if (enterEventMode || timeEvent || stateEvent) {
            
            if (!initialEventMode) {
                M_fmi3EnterEventMode(m);
            }
            
            // event iteration
            eventInfo.newDiscreteStatesNeeded = fmi3True;
            valuesOfContinuousStatesChanged   = fmi3False;
            nominalsOfContinuousStatesChanged = fmi3False;
            
            while (eventInfo.newDiscreteStatesNeeded) {
                
                // set inputs at super dense time point
                // M_fmi2SetReal/Integer/Boolean/String(m, ...)
                
                // update discrete states
                M_fmi3NewDiscreteStates(m, &eventInfo);
                
                // getOutput at super dense time point
                // M_fmi2GetReal/Integer/Boolean/String(m, ...)
                valuesOfContinuousStatesChanged |= eventInfo.valuesOfContinuousStatesChanged;
                nominalsOfContinuousStatesChanged |= eventInfo.nominalsOfContinuousStatesChanged;
                
                if (eventInfo.terminateSimulation) goto TERMINATE_MODEL;
            }
            

            // enter Continuous-Time Mode
            M_fmi3EnterContinuousTimeMode(m);

            // retrieve solution at simulation (re)start
            // M_fmi2GetReal/Integer/Boolean/String(m, ...)
            
            if (initialEventMode || valuesOfContinuousStatesChanged) {
                // the model signals a value change of states, retrieve them
                M_fmi3GetContinuousStates(m, x, nx);
            }

            if (initialEventMode || nominalsOfContinuousStatesChanged) {
                // the meaning of states has changed; retrieve new nominal values
                M_fmi3GetNominalsOfContinuousStates(m, x_nominal, nx);
            }

            if (eventInfo.nextEventTimeDefined) {
                tNext = min(eventInfo.nextEventTime, tEnd);
            } else {
                tNext = tEnd;
            }

            initialEventMode = fmi3False;
        }

        if (time >= tEnd) goto TERMINATE_MODEL;

        // compute derivatives
        M_fmi3GetDerivatives(m, der_x, nx);

        // advance time
        h = min(dt, tNext - time);
        time += h;
        M_fmi3SetTime(m, time);

        // set continuous inputs at t = time
        // M_fmi2SetReal(m, ...)

        // set states at t = time and perform one step
        // x = x + h * der_x // forward Euler method
        M_fmi3SetContinuousStates(m, x, nx);

        // get event indicators at t = time
        M_fmi3GetEventIndicators(m, z, nz);

        // detect  events, if any
        timeEvent = time >= tNext;
        // stateEvent = sign(z) <> sign(previous_z) or previous_z != 0 && z == 0
        // previous_z = z;

        // inform the model about an accepted step
        M_fmi3CompletedIntegratorStep(m, fmi3True, &enterEventMode, &terminateSimulation);
        
        // get continuous output
        // M_fmi2GetReal(m, ...)
    }
    
    // terminate simulation and retrieve final values
TERMINATE_MODEL:
    M_fmi3Terminate(m);
    // M_fmi2GetReal/Integer/Boolean/String(m, ...)
    
    // cleanup
    M_fmi3FreeInstance(m);

    printf("done.\n");

    return EXIT_SUCCESS;
}
