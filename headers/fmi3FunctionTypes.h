#ifndef fmi3FunctionTypes_h
#define fmi3FunctionTypes_h

#include "fmi3TypesPlatform.h"

/*
This header file must be utilized when compiling an FMU or an FMI master.
It declares data and function types for FMI 3.0-wg003.3

Copyright (C) 2011 MODELISAR consortium,
              2012-2018 Modelica Association Project "FMI"
              All rights reserved.

This file is licensed by the copyright holders under the 3-Clause BSD License
(https://opensource.org/licenses/BSD-3-Clause):

----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
- Neither the name of the copyright holders nor the names of its
 contributors may be used to endorse or promote products derived
 from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
----------------------------------------------------------------------------

with the extension:

You may distribute or publicly perform any modification only under the
terms of this license.
(Note, this means that if you distribute a modified file,
the modified file must also be provided under this license).
*/

#ifdef __cplusplus
extern "C" {
#endif

/* make sure all compiler use the same alignment policies for structures */
#if defined _MSC_VER || defined __GNUC__
#pragma pack(push,8)
#endif

/* Include stddef.h, in order that size_t etc. is defined */
#include <stddef.h>


/* Type definitions */
typedef enum {
    fmi3OK,
    fmi3Warning,
    fmi3Discard,
    fmi3Error,
    fmi3Fatal,
    fmi3Pending
} fmi3Status;

typedef enum {
    fmi3ModelExchange,
    fmi3CoSimulation
} fmi3Type;

typedef enum {
    fmi3DoStepStatus,
    fmi3PendingStatus,
    fmi3LastSuccessfulTime,
    fmi3Terminated
} fmi3StatusKind;

typedef enum {
    /* fmi3Independent = 0, not needed but reserved for future use */
    fmi3Constant  = 1,
    fmi3Fixed     = 2,
    fmi3Tunable   = 3,
    fmi3Discrete  = 4,
    fmi3Dependent = 5
} fmi3DependencyKind;

typedef void  (*fmi3CallbackLogger)         (fmi3ComponentEnvironment componentEnvironment, fmi3String instanceName, fmi3Status status, fmi3String category, fmi3String message, ...);
typedef void* (*fmi3CallbackAllocateMemory) (fmi3ComponentEnvironment componentEnvironment, size_t nobj, size_t size);
typedef void  (*fmi3CallbackFreeMemory)     (fmi3ComponentEnvironment componentEnvironment, void* obj);
typedef void  (*fmi3StepFinished)           (fmi3ComponentEnvironment componentEnvironment, fmi3Status status);

typedef struct {
    const fmi3CallbackLogger         logger;
    const fmi3CallbackAllocateMemory allocateMemory;
    const fmi3CallbackFreeMemory     freeMemory;
    const fmi3StepFinished           stepFinished;
    const fmi3ComponentEnvironment   componentEnvironment;
} fmi3CallbackFunctions;

typedef struct {
    fmi3Boolean newDiscreteStatesNeeded;
    fmi3Boolean terminateSimulation;
    fmi3Boolean nominalsOfContinuousStatesChanged;
    fmi3Boolean valuesOfContinuousStatesChanged;
    fmi3Boolean nextEventTimeDefined;
    fmi3Real    nextEventTime;
} fmi3EventInfo;

/* reset alignment policy to the one set before reading this file */
#if defined _MSC_VER || defined __GNUC__
#pragma pack(pop)
#endif

/* Define fmi3 function pointer types to simplify dynamic loading */

/***************************************************
Types for Common Functions
****************************************************/

/* Inquire version numbers of header files and setting logging status */
typedef const char* fmi3GetTypesPlatformTYPE(void);
typedef const char* fmi3GetVersionTYPE(void);
typedef fmi3Status  fmi3SetDebugLoggingTYPE(fmi3Component c, fmi3Boolean loggingOn, size_t nCategories, const fmi3String categories[]);

/* Creation and destruction of FMU instances and setting debug status */
typedef fmi3Component fmi3InstantiateTYPE (fmi3String instanceName, fmi3Type fmuType, fmi3String fmuGUID, fmi3String fmuResourceLocation, const fmi3CallbackFunctions* functions, fmi3Boolean visible, fmi3Boolean loggingOn);
typedef void          fmi3FreeInstanceTYPE(fmi3Component c);

/* Enter and exit initialization mode, terminate and reset */
typedef fmi3Status fmi3SetupExperimentTYPE        (fmi3Component c, fmi3Boolean toleranceDefined, fmi3Real tolerance, fmi3Real startTime, fmi3Boolean stopTimeDefined, fmi3Real stopTime);
typedef fmi3Status fmi3EnterInitializationModeTYPE(fmi3Component c);
typedef fmi3Status fmi3ExitInitializationModeTYPE (fmi3Component c);
typedef fmi3Status fmi3TerminateTYPE              (fmi3Component c);
typedef fmi3Status fmi3ResetTYPE                  (fmi3Component c);

/* Getting and setting variable values */
typedef fmi3Status fmi3GetRealTYPE   (fmi3Component c, const fmi3ValueReference vr[], size_t nvr, fmi3Real    value[], size_t nValues);
typedef fmi3Status fmi3GetIntegerTYPE(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, fmi3Integer value[], size_t nValues);
typedef fmi3Status fmi3GetBooleanTYPE(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, fmi3Boolean value[], size_t nValues);
typedef fmi3Status fmi3GetStringTYPE (fmi3Component c, const fmi3ValueReference vr[], size_t nvr, fmi3String  value[], size_t nValues);
typedef fmi3Status fmi3GetBinaryTYPE (fmi3Component c, const fmi3ValueReference vr[], size_t nvr, size_t size[], fmi3Binary value[], size_t nValues);

typedef fmi3Status fmi3SetRealTYPE   (fmi3Component c, const fmi3ValueReference vr[], size_t nvr, const fmi3Real    value[], size_t nValues);
typedef fmi3Status fmi3SetIntegerTYPE(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, const fmi3Integer value[], size_t nValues);
typedef fmi3Status fmi3SetBooleanTYPE(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, const fmi3Boolean value[], size_t nValues);
typedef fmi3Status fmi3SetStringTYPE (fmi3Component c, const fmi3ValueReference vr[], size_t nvr, const fmi3String  value[], size_t nValues);
typedef fmi3Status fmi3SetBinaryTYPE (fmi3Component c, const fmi3ValueReference vr[], size_t nvr, const size_t size[], const fmi3Binary [], size_t nValues);

/* Getting Variable Dependency Information */
typedef fmi3Status fmi3GetNumberOfVariableDependenciesTYPE(fmi3Component c, fmi3ValueReference vr, size_t* nDependencies);
typedef fmi3Status fmi3GetVariableDependenciesTYPE        (fmi3Component c, fmi3ValueReference vrDependent, size_t elementIndexDependent[], fmi3ValueReference vrIndependent[], size_t elementIndexIndependent[], fmi3DependencyKind dependencyType[], size_t nDependencies);

/* Getting and setting the internal FMU state */
typedef fmi3Status fmi3GetFMUstateTYPE           (fmi3Component c, fmi3FMUstate* FMUstate);
typedef fmi3Status fmi3SetFMUstateTYPE           (fmi3Component c, fmi3FMUstate  FMUstate);
typedef fmi3Status fmi3FreeFMUstateTYPE          (fmi3Component c, fmi3FMUstate* FMUstate);
typedef fmi3Status fmi3SerializedFMUstateSizeTYPE(fmi3Component c, fmi3FMUstate  FMUstate, size_t* size);
typedef fmi3Status fmi3SerializeFMUstateTYPE     (fmi3Component c, fmi3FMUstate  FMUstate, fmi3Byte serializedState[], size_t size);
typedef fmi3Status fmi3DeSerializeFMUstateTYPE   (fmi3Component c, const fmi3Byte serializedState[], size_t size, fmi3FMUstate* FMUstate);

/* Getting partial derivatives */
typedef fmi3Status fmi3GetDirectionalDerivativeTYPE(fmi3Component c, const fmi3ValueReference vrUnknown[], size_t nUnknown, const fmi3ValueReference vrKnown[], size_t nKnown, const fmi3Real dvKnown[], size_t nDvKnown, fmi3Real dvUnknown[], size_t nDvUnknown);

/***************************************************
Types for Functions for FMI3 for Model Exchange
****************************************************/

/* Enter and exit the different modes */
typedef fmi3Status fmi3EnterEventModeTYPE         (fmi3Component c);
typedef fmi3Status fmi3NewDiscreteStatesTYPE      (fmi3Component c, fmi3EventInfo* fmi3eventInfo);
typedef fmi3Status fmi3EnterContinuousTimeModeTYPE(fmi3Component c);
typedef fmi3Status fmi3CompletedIntegratorStepTYPE(fmi3Component c, fmi3Boolean noSetFMUStatePriorToCurrentPoint, fmi3Boolean* enterEventMode, fmi3Boolean* terminateSimulation);

/* Providing independent variables and re-initialization of caching */
typedef fmi3Status fmi3SetTimeTYPE            (fmi3Component c, fmi3Real time);
typedef fmi3Status fmi3SetContinuousStatesTYPE(fmi3Component c, const fmi3Real x[], size_t nx);

/* Evaluation of the model equations */
typedef fmi3Status fmi3GetDerivativesTYPE               (fmi3Component c, fmi3Real derivatives[], size_t nx);
typedef fmi3Status fmi3GetEventIndicatorsTYPE           (fmi3Component c, fmi3Real eventIndicators[], size_t ni);
typedef fmi3Status fmi3GetContinuousStatesTYPE          (fmi3Component c, fmi3Real x[], size_t nx);
typedef fmi3Status fmi3GetNominalsOfContinuousStatesTYPE(fmi3Component c, fmi3Real x_nominal[], size_t nx);

typedef fmi3Status fmi3GetNumberOfEventIndicatorsTYPE   (fmi3Component c, size_t* nz);
typedef fmi3Status fmi3GetNumberOfContinuousStatesTYPE  (fmi3Component c, size_t* nx);

/***************************************************
Types for Functions for FMI3 for Co-Simulation
****************************************************/

/* Simulating the slave */
typedef fmi3Status fmi3SetRealInputDerivativesTYPE (fmi3Component c, const fmi3ValueReference vr[], size_t nvr, const fmi3Integer order[], const fmi3Real value[], size_t nValues);
typedef fmi3Status fmi3GetRealOutputDerivativesTYPE(fmi3Component c, const fmi3ValueReference vr[], size_t nvr, const fmi3Integer order[],       fmi3Real value[], size_t nValues);

typedef fmi3Status fmi3DoStepTYPE    (fmi3Component c, fmi3Real currentCommunicationPoint, fmi3Real communicationStepSize, fmi3Boolean noSetFMUStatePriorToCurrentPoint);
typedef fmi3Status fmi3CancelStepTYPE(fmi3Component c);

/* Inquire slave status */
typedef fmi3Status fmi3GetStatusTYPE       (fmi3Component c, const fmi3StatusKind s, fmi3Status*  value);
typedef fmi3Status fmi3GetRealStatusTYPE   (fmi3Component c, const fmi3StatusKind s, fmi3Real*    value);
typedef fmi3Status fmi3GetIntegerStatusTYPE(fmi3Component c, const fmi3StatusKind s, fmi3Integer* value);
typedef fmi3Status fmi3GetBooleanStatusTYPE(fmi3Component c, const fmi3StatusKind s, fmi3Boolean* value);
typedef fmi3Status fmi3GetStringStatusTYPE (fmi3Component c, const fmi3StatusKind s, fmi3String*  value);

#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif /* fmi3FunctionTypes_h */
