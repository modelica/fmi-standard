#ifndef fmi3FunctionTypes_h
#define fmi3FunctionTypes_h

#include "fmi3TypesPlatform.h"

/*
This header file must be utilized when compiling an FMU or an FMI master.
It declares data and function types for FMI 3.0-wg003.3

Copyright (C) 2011 MODELISAR consortium,
              2012-2018 Modelica Association Project "FMI"
              All rights reserved.
This file is licensed by the copyright holders under the BSD 2-Clause License
(http://www.opensource.org/licenses/bsd-license.html):

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

typedef void  (*fmi3CallbackLogger)         (fmi3ComponentEnvironment, fmi3String, fmi3Status, fmi3String, fmi3String, ...);
typedef void* (*fmi3CallbackAllocateMemory) (fmi3ComponentEnvironment, size_t, size_t);
typedef void  (*fmi3CallbackFreeMemory)     (fmi3ComponentEnvironment, void*);
typedef void  (*fmi3StepFinished)           (fmi3ComponentEnvironment, fmi3Status);

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
typedef fmi3Status  fmi3SetDebugLoggingTYPE(fmi3Component, fmi3Boolean, size_t, const fmi3String[]);

/* Creation and destruction of FMU instances and setting debug status */
typedef fmi3Component fmi3InstantiateTYPE (fmi3String, fmi3Type, fmi3String, fmi3String, const fmi3CallbackFunctions*, fmi3Boolean, fmi3Boolean);
typedef void          fmi3FreeInstanceTYPE(fmi3Component);

/* Enter and exit initialization mode, terminate and reset */
typedef fmi3Status fmi3SetupExperimentTYPE        (fmi3Component, fmi3Boolean, fmi3Real, fmi3Real, fmi3Boolean, fmi3Real);
typedef fmi3Status fmi3EnterInitializationModeTYPE(fmi3Component);
typedef fmi3Status fmi3ExitInitializationModeTYPE (fmi3Component);
typedef fmi3Status fmi3TerminateTYPE              (fmi3Component);
typedef fmi3Status fmi3ResetTYPE                  (fmi3Component);

/* Getting and setting variable values */
typedef fmi3Status fmi3GetRealTYPE   (fmi3Component, const fmi3ValueReference[], size_t, fmi3Real   [], size_t);
typedef fmi3Status fmi3GetIntegerTYPE(fmi3Component, const fmi3ValueReference[], size_t, fmi3Integer[], size_t);
typedef fmi3Status fmi3GetBooleanTYPE(fmi3Component, const fmi3ValueReference[], size_t, fmi3Boolean[], size_t);
typedef fmi3Status fmi3GetStringTYPE (fmi3Component, const fmi3ValueReference[], size_t, fmi3String [], size_t);
typedef fmi3Status fmi3GetBinaryTYPE (fmi3Component, const fmi3ValueReference[], size_t, size_t [], fmi3Binary [], size_t);

typedef fmi3Status fmi3SetRealTYPE   (fmi3Component, const fmi3ValueReference[], size_t, const fmi3Real   [], size_t);
typedef fmi3Status fmi3SetIntegerTYPE(fmi3Component, const fmi3ValueReference[], size_t, const fmi3Integer[], size_t);
typedef fmi3Status fmi3SetBooleanTYPE(fmi3Component, const fmi3ValueReference[], size_t, const fmi3Boolean[], size_t);
typedef fmi3Status fmi3SetStringTYPE (fmi3Component, const fmi3ValueReference[], size_t, const fmi3String [], size_t);
typedef fmi3Status fmi3SetBinaryTYPE (fmi3Component, const fmi3ValueReference[], size_t, const size_t [], const fmi3Binary [], size_t);

/* Getting Variable Dependency Information */
typedef fmi3Status fmi3GetNumberOfVariableDependenciesTYPE(fmi3Component, fmi3ValueReference, size_t*);
typedef fmi3Status fmi3GetVariableDependenciesTYPE        (fmi3Component, fmi3ValueReference, size_t [], fmi3ValueReference [], size_t [], fmi3DependencyKind [], size_t);

/* Getting and setting the internal FMU state */
typedef fmi3Status fmi3GetFMUstateTYPE           (fmi3Component, fmi3FMUstate*);
typedef fmi3Status fmi3SetFMUstateTYPE           (fmi3Component, fmi3FMUstate);
typedef fmi3Status fmi3FreeFMUstateTYPE          (fmi3Component, fmi3FMUstate*);
typedef fmi3Status fmi3SerializedFMUstateSizeTYPE(fmi3Component, fmi3FMUstate, size_t*);
typedef fmi3Status fmi3SerializeFMUstateTYPE     (fmi3Component, fmi3FMUstate, fmi3Byte[], size_t);
typedef fmi3Status fmi3DeSerializeFMUstateTYPE   (fmi3Component, const fmi3Byte[], size_t, fmi3FMUstate*);

/* Getting partial derivatives */
typedef fmi3Status fmi3GetDirectionalDerivativeTYPE(fmi3Component, const fmi3ValueReference[], size_t,
                                                                   const fmi3ValueReference[], size_t,
                                                                   const fmi3Real[], size_t, fmi3Real[], size_t);

/***************************************************
Types for Functions for FMI3 for Model Exchange
****************************************************/

/* Enter and exit the different modes */
typedef fmi3Status fmi3EnterEventModeTYPE         (fmi3Component);
typedef fmi3Status fmi3NewDiscreteStatesTYPE      (fmi3Component, fmi3EventInfo*);
typedef fmi3Status fmi3EnterContinuousTimeModeTYPE(fmi3Component);
typedef fmi3Status fmi3CompletedIntegratorStepTYPE(fmi3Component, fmi3Boolean, fmi3Boolean*, fmi3Boolean*);

/* Providing independent variables and re-initialization of caching */
typedef fmi3Status fmi3SetTimeTYPE            (fmi3Component, fmi3Real);
typedef fmi3Status fmi3SetContinuousStatesTYPE(fmi3Component, const fmi3Real[], size_t);

/* Evaluation of the model equations */
typedef fmi3Status fmi3GetDerivativesTYPE               (fmi3Component, fmi3Real[], size_t);
typedef fmi3Status fmi3GetEventIndicatorsTYPE           (fmi3Component, fmi3Real[], size_t);
typedef fmi3Status fmi3GetContinuousStatesTYPE          (fmi3Component, fmi3Real[], size_t);
typedef fmi3Status fmi3GetNominalsOfContinuousStatesTYPE(fmi3Component, fmi3Real[], size_t);

typedef fmi3Status fmi3GetNumberOfEventIndicatorsTYPE   (fmi3Component, size_t*);
typedef fmi3Status fmi3GetNumberOfContinuousStatesTYPE  (fmi3Component, size_t*);

/***************************************************
Types for Functions for FMI3 for Co-Simulation
****************************************************/

/* Simulating the slave */
typedef fmi3Status fmi3SetRealInputDerivativesTYPE (fmi3Component, const fmi3ValueReference [], size_t, const fmi3Integer [], const fmi3Real [], size_t);
typedef fmi3Status fmi3GetRealOutputDerivativesTYPE(fmi3Component, const fmi3ValueReference [], size_t, const fmi3Integer [],       fmi3Real [], size_t);

typedef fmi3Status fmi3DoStepTYPE    (fmi3Component, fmi3Real, fmi3Real, fmi3Boolean);
typedef fmi3Status fmi3CancelStepTYPE(fmi3Component);

/* Inquire slave status */
typedef fmi3Status fmi3GetStatusTYPE       (fmi3Component, const fmi3StatusKind, fmi3Status* );
typedef fmi3Status fmi3GetRealStatusTYPE   (fmi3Component, const fmi3StatusKind, fmi3Real*   );
typedef fmi3Status fmi3GetIntegerStatusTYPE(fmi3Component, const fmi3StatusKind, fmi3Integer*);
typedef fmi3Status fmi3GetBooleanStatusTYPE(fmi3Component, const fmi3StatusKind, fmi3Boolean*);
typedef fmi3Status fmi3GetStringStatusTYPE (fmi3Component, const fmi3StatusKind, fmi3String* );

#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif /* fmi3FunctionTypes_h */
