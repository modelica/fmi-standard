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

static int sign(double x) {
    return (x > 0) - (x < 0);
}

static void checkStatus() {

    fmi3Instance m;
    fmi3Float64 *der_x;
    size_t nx;
    fmi3Status status;

// tag::CheckStatus[]
status = M_fmi3GetDerivatives(m, der_x, nx);

switch (status) {
    case fmi3Discard:
        // reduce step size and try again
        break;
    case fmi3Error:
        // clean up and stop simulation
        break;
    case fmi3Fatal:
        // stop using the model
        break;
    default:
        break;
}
// end::CheckStatus[]
}

int main(int argc, char* argv[]) {

    fmi3Float64 h = 0.1, tNext = h, tEnd = 10, time, tStart = 0;
    fmi3Boolean timeEvent, stateEvent, enterEventMode, terminateSimulation = fmi3False, initialEventMode, valuesOfContinuousStatesChanged, nominalsOfContinuousStatesChanged;
    fmi3EventInfo eventInfo;

    const char *guid = "{8c4e810f-3da3-4a00-8276-176fa3c9f000}";

    fmi3CallbackFunctions callbacks = { NULL };
    
    callbacks.allocateMemory = cb_allocateMemory;
    callbacks.freeMemory     = cb_freeMemory;
    callbacks.logMessage     = cb_logMessage;
    
    fmi3Instance m;
    fmi3Float64 x[2], x_nominal[2], der_x[2], dt = 0.01, z[1], previous_z[1];
    size_t nx = 2, nz = 0, i;

    printf("Running model_exchange example... ");

// tag::ModelExchange[]
m = M_fmi3Instantiate("m", fmi3ModelExchange, guid, NULL, &callbacks, fmi3False, fmi3False, NULL);
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

// initialize previous event indicators
M_fmi3GetEventIndicators(m, previous_z, nz);

M_fmi3EnterContinuousTimeMode(m);

// retrieve initial state x and
// nominal values of x (if absolute tolerance is needed)
M_fmi3GetContinuousStates(m, x, nx);
M_fmi3GetNominalsOfContinuousStates(m, x_nominal, nx);

// retrieve solution at t=Tstart, for example, for outputs
// M_fmi3SetFloat*/Int*/UInt*/Boolean/String/Binary(m, ...)

while (!terminateSimulation) {

    tNext = time + h;

    // handle events
    if (enterEventMode || stateEvent || timeEvent) {

        if (!initialEventMode) {
            // TODO: pass rootsFound
            M_fmi3EnterEventMode(m, fmi3False, fmi3False, NULL, 0, timeEvent);
        }

        // event iteration
        eventInfo.newDiscreteStatesNeeded = fmi3True;
        valuesOfContinuousStatesChanged   = fmi3False;
        nominalsOfContinuousStatesChanged = fmi3False;

        while (eventInfo.newDiscreteStatesNeeded) {

            // set inputs at super dense time point
            // M_fmi3SetFloat*/Int*/UInt*/Boolean/String/Binary(m, ...)

            // update discrete states
            M_fmi3NewDiscreteStates(m, &eventInfo);

            // getOutput at super dense time point
            // M_fmi3GetFloat*/Int*/UInt*/Boolean/String/Binary(m, ...)
            valuesOfContinuousStatesChanged |= eventInfo.valuesOfContinuousStatesChanged;
            nominalsOfContinuousStatesChanged |= eventInfo.nominalsOfContinuousStatesChanged;

            if (eventInfo.terminateSimulation) goto TERMINATE_MODEL;
        }


        // enter Continuous-Time Mode
        M_fmi3EnterContinuousTimeMode(m);

        // retrieve solution at simulation (re)start
        // M_fmi3GetFloat*/Int*/UInt*/Boolean/String/Binary(m, ...)

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

    if (time >= tEnd) {
        goto TERMINATE_MODEL;
    }

    // compute derivatives
    M_fmi3GetDerivatives(m, der_x, nx);

    // advance time
    h = min(dt, tNext - time);
    time += h;
    M_fmi3SetTime(m, time);

    // set continuous inputs at t = time
    // M_fmi3SetFloat*(m, ...)

    // set states at t = time and perform one step
    for (i = 0; i < nx; i++) {
        x[i] += h * der_x[i]; // forward Euler method
    }

    M_fmi3SetContinuousStates(m, x, nx);

    // get event indicators at t = time
    M_fmi3GetEventIndicators(m, z, nz);

    stateEvent = fmi3False;

    for (i = 0; i < nz; i++) {
        stateEvent |= sign(z[i]) != sign(previous_z[i]); // detect events
        previous_z[i] = z[i]; // remember the current value
    }

    // inform the model about an accepted step
    M_fmi3CompletedIntegratorStep(m, fmi3True, &enterEventMode, &terminateSimulation);

    // get continuous output
    // M_fmi3GetFloat*(m, ...)
}

TERMINATE_MODEL:

// terminate simulation and retrieve final values
M_fmi3Terminate(m);

// M_fmi3GetFloat*/Int*/UInt*/Boolean/String/Binary(m, ...)

// cleanup
M_fmi3FreeInstance(m);
// end::ModelExchange[]

    printf("done.\n");

    return EXIT_SUCCESS;
}
