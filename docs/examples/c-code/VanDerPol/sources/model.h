#ifndef model_h
#define model_h

#if FMI_VERSION != 1 && FMI_VERSION != 2 && FMI_VERSION != 3
#error FMI_VERSION must be one of 1, 2 or 3
#endif

#include <stddef.h>  // for size_t
#include <stdbool.h> // for bool

#include "config.h"

#if FMI_VERSION == 1

#define not_modelError (modelInstantiated|modelInitialized|modelTerminated)

typedef enum {
    modelInstantiated = 1<<0,
    modelInitialized  = 1<<1,
    modelTerminated   = 1<<2,
    modelError        = 1<<3
} ModelState;

#else

typedef enum {
    modelStartAndEnd        = 1<<0,
    modelInstantiated       = 1<<1,
    modelInitializationMode = 1<<2,

    // ME states
    modelEventMode          = 1<<3,
    modelContinuousTimeMode = 1<<4,
    
    // CS states
    modelStepComplete       = 1<<5,
    modelStepInProgress     = 1<<6,
    modelStepFailed         = 1<<7,
    modelStepCanceled       = 1<<8,

    modelTerminated         = 1<<9,
    modelError              = 1<<10,
    modelFatal              = 1<<11,
} ModelState;

#endif

typedef enum {
    ModelExchange,
    CoSimulation
} InterfaceType;

typedef enum {
    OK,
    Warning,
    Discard,
    Error,
    Fatal,
    Pending
} Status;

#if FMI_VERSION < 3
typedef void  (*loggerType)        (void *componentEnvironment, const char *instanceName, int status, const char *category, const char *message, ...);
typedef void* (*allocateMemoryType)(size_t nobj, size_t size);
typedef void  (*freeMemoryType)    (void *obj);
#else
typedef void  (*loggerType)        (void *componentEnvironment, const char *instanceName, int status, const char *category, const char *message);
typedef void* (*allocateMemoryType)(void *componentEnvironment, size_t nobj, size_t size);
typedef void  (*freeMemoryType)    (void *componentEnvironment, void *obj);
#endif

typedef struct {
    
    double time;
    const char *instanceName;
    InterfaceType type;
    const char *resourceLocation;

	// callback functions
	loggerType logger;
	allocateMemoryType allocateMemory;
	freeMemoryType freeMemory;

    bool logEvents;
    bool logErrors;

    void *componentEnvironment;
    ModelState state;
    
    // event info
    bool newDiscreteStatesNeeded;
    bool terminateSimulation;
    bool nominalsOfContinuousStatesChanged;
    bool valuesOfContinuousStatesChanged;
    bool nextEventTimeDefined;
    double nextEventTime;
    
    bool isDirtyValues;
    bool isNewEventIteration;
    
    ModelData *modelData;

	// event indicators
	double *z;
	double *prez;
    
} ModelInstance;

ModelInstance *createModelInstance(
	loggerType logger,
	allocateMemoryType allocateMemory,
	freeMemoryType freeMemory,
	void *componentEnvironment,
	const char *instanceName,
	const char *GUID,
	const char *resourceLocation,
	bool loggingOn,
	InterfaceType interfaceType);
void freeModelInstance(ModelInstance *comp);

void setStartValues(ModelInstance *comp);
void calculateValues(ModelInstance *comp);
    
Status getFloat64(ModelInstance* comp, ValueReference vr, double *value, size_t *index);
Status getInt32(ModelInstance* comp, ValueReference vr, int *value, size_t *index);
Status getBoolean(ModelInstance* comp, ValueReference vr, bool *value, size_t *index);
Status getString(ModelInstance* comp, ValueReference vr, const char **value, size_t *index);
Status getBinary(ModelInstance* comp, ValueReference vr, size_t size[], const char *value[], size_t *index);

Status setFloat64(ModelInstance* comp, ValueReference vr, const double *value, size_t *index);
Status setInt32(ModelInstance* comp, ValueReference vr, const int *value, size_t *index);
Status setBoolean(ModelInstance* comp, ValueReference vr, const bool *value, size_t *index);
Status setString(ModelInstance* comp, ValueReference vr, const char *const *value, size_t *index);
Status setBinary(ModelInstance* comp, ValueReference vr, const size_t size[], const char *const value[], size_t *index);

void getContinuousStates(ModelInstance *comp, double x[], size_t nx);
void setContinuousStates(ModelInstance *comp, const double x[], size_t nx);
void getDerivatives(ModelInstance *comp, double dx[], size_t nx);
double getPartialDerivative(ModelInstance *comp, ValueReference unknown, ValueReference known);
void getEventIndicators(ModelInstance *comp, double z[], size_t nz);
void eventUpdate(ModelInstance *comp);

void *allocateMemory(ModelInstance *comp, size_t num, size_t size);
void freeMemory(ModelInstance *comp, void *obj);
const char *duplicateString(ModelInstance *comp, const char *str1);
bool invalidNumber(ModelInstance *comp, const char *f, const char *arg, size_t actual, size_t expected);
bool invalidState(ModelInstance *comp, const char *f, int statesExpected);
bool nullPointer(ModelInstance* comp, const char *f, const char *arg, const void *p);
void logError(ModelInstance *comp, const char *message, ...);
Status setDebugLogging(ModelInstance *comp, bool loggingOn, size_t nCategories, const char * const categories[]);
void logEvent(ModelInstance *comp, const char *message, ...);
void logError(ModelInstance *comp, const char *message, ...);

// shorthand to access the variables
#define M(v) (comp->modelData->v)

#define GET_VARIABLES(T) \
size_t index = 0; \
Status status = OK; \
for (int i = 0; i < nvr; i++) { \
    Status s = get ## T(comp, vr[i], value, &index); \
    status = max(status, s); \
    if (status > Warning) return status; \
} \
return status;

#define SET_VARIABLES(T) \
size_t index = 0; \
Status status = OK; \
for (int i = 0; i < nvr; i++) { \
    Status s = set ## T(comp, vr[i], value, &index); \
    status = max(status, s); \
    if (status > Warning) return status; \
} \
if (nvr > 0) comp->isDirtyValues = true; \
return status;

// TODO: make this work with arrays
#define GET_BOOLEAN_VARIABLES \
Status status = OK; \
for (int i = 0; i < nvr; i++) { \
    bool v = false; \
    size_t index = 0; \
    Status s = getBoolean(comp, vr[i], &v, &index); \
    value[i] = v; \
    status = max(status, s); \
    if (status > Warning) return status; \
} \
return status;

// TODO: make this work with arrays
#define SET_BOOLEAN_VARIABLES \
Status status = OK; \
for (int i = 0; i < nvr; i++) { \
    bool v = value[i]; \
    size_t index = 0; \
    Status s = setBoolean(comp, vr[i], &v, &index); \
    status = max(status, s); \
    if (status > Warning) return status; \
} \
return status;

#endif  /* model_h */
