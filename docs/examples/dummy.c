#include "fmi3Functions.h"

int main(int argc, char* argv[]) {

    {
        // tag::AlgebraicLoops1[]
        fmi3Instance FMU1, FMU2;
        fmi3ValueReference vr_FMU1_u, vr_FMU1_y, vr_FMU2_u1, vr_FMU2_u2, vr_FMU2_y1, vr_FMU2_y2;
        fmi3Float64 s = 0.1, FMU2_y1, FMU1_y, FMU2_y2;
        // ...
        fmi3SetFloat64(FMU2, &vr_FMU2_u1, 1, &s, 1);
        fmi3GetFloat64(FMU2, &vr_FMU2_y1, 1, &FMU2_y1, 1);
        fmi3SetFloat64(FMU1, &vr_FMU1_u, 1, &FMU2_y1, 1);
        fmi3GetFloat64(FMU1, &vr_FMU1_y, 1, &FMU1_y, 1);
        fmi3SetFloat64(FMU2, &vr_FMU2_u2, 1, &FMU1_y, 1);
        fmi3GetFloat64(FMU2, &vr_FMU2_y1, 1, &FMU2_y2, 1);
        //...
        // end::AlgebraicLoops1[]
    }

    /*************/

    {
        fmi3Instance FMU3, FMU4;
        fmi3ValueReference vr_FMU3_u, vr_FMU3_y, vr_FMU4_u, vr_FMU4_y;
        fmi3Float64 s, FMU3_y, FMU4_y, residual, tolerance;
        bool converged = false;

        while (!converged) { // start iteration
            // s determined by the solver
            // ...
            fmi3SetFloat64(FMU4, &vr_FMU4_u, 1, &s, 1);
            fmi3GetFloat64(FMU4, &vr_FMU4_y, 1, &FMU4_y, 1);
            fmi3SetFloat64(FMU3, &vr_FMU3_u, 1, &FMU4_y, 1);
            fmi3GetFloat64(FMU3, &vr_FMU3_y, 1, &FMU3_y, 1);
            residual = s - FMU3_y; // provided to the solver
            converged = residual < tolerance;
        }
    }

    /*************/



    return 0;
}
