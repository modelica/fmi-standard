#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#define FMI3_FUNCTION_PREFIX s1_
#include "fmi3Functions.h"
#undef FMI3_FUNCTION_PREFIX

#undef fmi3Functions_h
#define FMI3_FUNCTION_PREFIX s2_
#include "fmi3Functions.h"
#undef FMI3_FUNCTION_PREFIX


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

    fmi3Float64 startTime, stopTime, h, tc, lastSuccessfulTime;
    fmi3Status status = fmi3OK;
    fmi3Boolean discard;
    bool terminateSimulation = false;
    
    const char *guid = "{8c4e810f-3da3-4a00-8276-176fa3c9f000}";
    
    fmi3CallbackFunctions callbacks = { NULL };
    fmi3Instance s1, s2;

    printf("Running CoSimulation example... ");

// tag::CoSimulation[]
////////////////////////////
// Initialization sub-phase

// set callback functions
callbacks.logMessage     = cb_logMessage;
callbacks.allocateMemory = cb_allocateMemory;
callbacks.freeMemory     = cb_freeMemory;

// instantiate both slaves
s1 = s1_fmi3Instantiate("slave1", fmi3CoSimulation, guid, NULL, &callbacks, fmi3False, fmi3False, NULL);
s2 = s2_fmi3Instantiate("slave2", fmi3CoSimulation, guid, NULL, &callbacks, fmi3False, fmi3False, NULL);

if (s1 == NULL || s2 == NULL)
    return EXIT_FAILURE;

// start and stop time
startTime = 0;
stopTime = 10;

// communication step size
h = 0.01;

// set all variable start values (of "ScalarVariable / <type> / start")
// s1_fmi3SetReal/Integer/Boolean/String(s1, ...);
// s2_fmi3SetReal/Integer/Boolean/String(s2, ...);

// initialize the slaves
s1_fmi3SetupExperiment(s1, fmi3False, 0.0, startTime, fmi3True, stopTime);
s2_fmi3SetupExperiment(s2, fmi3False, 0.0, startTime, fmi3True, stopTime);

s1_fmi3EnterInitializationMode(s1);
s2_fmi3EnterInitializationMode(s2);

// set the input values at time = startTime
// fmi3SetReal/Integer/Boolean/String(s1, ...);
// fmi3SetReal/Integer/Boolean/String(s2, ...);

s1_fmi3ExitInitializationMode(s1);
s2_fmi3ExitInitializationMode(s2);

////////////////////////
// Simulation sub-phase
tc = startTime; // current master time

while ((tc < stopTime) && (status == fmi3OK)) {
    
    // retrieve outputs
    // fmi3GetReal(s1, ..., 1, &y1);
    // fmi3GetReal(s2, ..., 1, &y2);

    // set inputs
    // fmi3SetReal(s1, ..., 1, &y2);
    // fmi3SetReal(s2, ..., 1, &y1);

    // call slave s1 and check status
    status = s1_fmi3DoStep(s1, tc, h, fmi3True, NULL);

    switch (status) {
        case fmi3Discard:
            s1_fmi3GetDoStepDiscardedStatus(s1, &discard, &lastSuccessfulTime);
            if (discard == fmi3True)
                printf("Slave s1 wants to terminate simulation.");
        case fmi3Error:
        case fmi3Fatal:
            terminateSimulation = true;
            break;
        default:
            break;
    }
    
    if (terminateSimulation)
        break;

    // call slave s2 and check status as above
    status = s2_fmi3DoStep(s2, tc, h, fmi3True, NULL);

    // ...

    // increment master time
    tc += h;
 }

//////////////////////////
// Shutdown sub-phase
if (status != fmi3Error && status != fmi3Fatal) {
    s1_fmi3Terminate(s1);
    s2_fmi3Terminate(s2);
}

if (status != fmi3Fatal) {
    s1_fmi3FreeInstance(s1);
    s2_fmi3FreeInstance(s2);
}
// end::CoSimulation[]

    printf("done.\n");

    return EXIT_SUCCESS;
}
