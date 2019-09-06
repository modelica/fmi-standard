#ifndef config_h
#define config_h

// define class name and unique id
#define MODEL_IDENTIFIER VanDerPol
#define MODEL_GUID "{8c4e810f-3da3-4a00-8276-176fa3c9f000}"

// define model size
#define NUMBER_OF_STATES 2
#define NUMBER_OF_EVENT_INDICATORS 0

#define GET_FLOAT64
#define SET_FLOAT64

#define GET_PARTIAL_DERIVATIVE

#define FIXED_SOLVER_STEP 1e-2

typedef enum {
    vr_x0, vr_der_x0, vr_x1, vr_der_x1, vr_mu
} ValueReference;

typedef struct {

    double x0;
    double der_x0;
    double x1;
    double der_x1;
    double mu;

} ModelData;

#endif /* config_h */
