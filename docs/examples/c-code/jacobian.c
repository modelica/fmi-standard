#include <stdio.h>
#include <assert.h>
#include "fmi3Functions.h"


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

    int i = 0;
    fmi3Float64 time = 0;
    int nx = 2;
    fmi3Float64 x[2];
    fmi3ValueReference vr_x[]  = { 0, 2 };
    fmi3ValueReference vr_dx[] = { 1, 3 };
    fmi3Float64 dk = 1;
    fmi3Float64 J[2][2];
    fmi3CallbackFunctions callbacks = { NULL };
    fmi3Instance m;
    
    callbacks.allocateMemory = cb_allocateMemory;
    callbacks.freeMemory     = cb_freeMemory;
    callbacks.logMessage     = cb_logMessage;

    printf("Running Jacobian example... ");

    m = fmi3Instantiate("jacobian", fmi3ModelExchange, "{8c4e810f-3da3-4a00-8276-176fa3c9f000}", NULL, &callbacks, fmi3False, fmi3False, NULL);

    fmi3SetupExperiment(m, fmi3False, 0, 0, fmi3False, 0);

    fmi3EnterInitializationMode(m);
    fmi3ExitInitializationMode(m);

    fmi3EnterContinuousTimeMode(m);

    fmi3GetContinuousStates(m, x, nx);

// tag::GetJacobian[]
// from the XML file:
//   nx       number of states
//   vr_x[]   value references of continuous states
//   vr_xd[]  value references of state derivatives
//
// variables:
//   m        model instance
//   x[]      continuous states
//   dk = 1   delta knowns
//   J[][]    Jacobian (transposed for simplicity)

// set time, states and inputs
fmi3SetTime(m, time);
fmi3SetContinuousStates(m, x, nx);
// fmi3SetFloat*/Int*/UInt*/Boolean/String/Binary(m, ...)

// if required at this step, compute the Jacobian as a dense matrix
for (i = 0; i < nx; i++) {
    // construct the Jacobian matrix column wise
    fmi3GetDirectionalDerivative(m, vr_dx, nx, &vr_x[i], 1, &dk, 1, &J[i][0], nx);
}
// end::GetJacobian[]

    assert(J[0][0] ==  0);
    assert(J[0][1] == -1);
    assert(J[1][0] ==  1);
    assert(J[1][1] == -3);

    fmi3Terminate(m);
    fmi3FreeInstance(m);

    printf("done.\n");

    return 0;
}
