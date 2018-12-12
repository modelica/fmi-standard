#ifndef fmi3FunctionTypes_h
#define fmi3FunctionTypes_h

#include "fmi3TypesPlatform.h"

/*
This header file must be utilized when compiling an FMU or an FMI master.
It declares data and function types for FMI 3.0-wg003.3

Copyright (C) 2011 MODELISAR consortium,
              2012-2018 Modelica Association Project "FMI"
              All rights reserved.

This file is licensed by the copyright holders under the 2-Clause BSD License
(https://opensource.org/licenses/BSD-2-Clause):

----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

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

/* tag::Status[] */
typedef enum {
    fmi3OK,
    fmi3Warning,
    fmi3Discard,
    fmi3Error,
    fmi3Fatal,
    fmi3Pending
} fmi3Status;
/* end::Status[] */

/* tag::Type[] */
typedef enum {
    fmi3ModelExchange,
    fmi3CoSimulation
} fmi3Type;
/* end::Type[] */

/* tag::DependencyKind[] */
typedef enum {
    /* fmi3Independent = 0, not needed but reserved for future use */
    fmi3Constant  = 1,
    fmi3Fixed     = 2,
    fmi3Tunable   = 3,
    fmi3Discrete  = 4,
    fmi3Dependent = 5
} fmi3DependencyKind;
/* end::DependencyKind[] */

/* tag::CallbackFunctions[] */
typedef void  (*fmi3CallbackLogger)         (fmi3ComponentEnvironment componentEnvironment,
                                             fmi3String instanceName,
                                             fmi3Status status,
                                             fmi3String category,
                                             fmi3String message);
typedef void* (*fmi3CallbackAllocateMemory) (fmi3ComponentEnvironment componentEnvironment,
                                             size_t nobj,
                                             size_t size);
typedef void  (*fmi3CallbackFreeMemory)     (fmi3ComponentEnvironment componentEnvironment,
                                             void* obj);
typedef void  (*fmi3StepFinished)           (fmi3ComponentEnvironment componentEnvironment,
                                             fmi3Status status);

typedef struct {
    fmi3CallbackLogger         logger;
    fmi3CallbackAllocateMemory allocateMemory;
    fmi3CallbackFreeMemory     freeMemory;
    fmi3StepFinished           stepFinished;
    fmi3ComponentEnvironment   componentEnvironment;
} fmi3CallbackFunctions;
/* end::CallbackFunctions[] */

/* tag::EventInfo[] */
typedef struct {
    fmi3Boolean newDiscreteStatesNeeded;
    fmi3Boolean terminateSimulation;
    fmi3Boolean nominalsOfContinuousStatesChanged;
    fmi3Boolean valuesOfContinuousStatesChanged;
    fmi3Boolean nextEventTimeDefined;
    fmi3Float64 nextEventTime;  /* next event if nextEventTimeDefined=fmi3True */
} fmi3EventInfo;
/* end::EventInfo[] */

/* reset alignment policy to the one set before reading this file */
#if defined _MSC_VER || defined __GNUC__
#pragma pack(pop)
#endif

/* Define fmi3 function pointer types to simplify dynamic loading */

/***************************************************
Types for Common Functions
****************************************************/

/* Inquire version numbers and setting logging status */

/* tag::GetVersion[] */
typedef const char* fmi3GetVersionTYPE(void);
/* end::GetVersion[] */

/* tag::SetDebugLogging[] */
typedef fmi3Status  fmi3SetDebugLoggingTYPE(fmi3Component c,
                                            fmi3Boolean loggingOn,
                                            size_t nCategories,
                                            const fmi3String categories[]);
/* end::SetDebugLogging[] */

/* Creation and destruction of FMU instances and setting debug status */
/* tag::Instantiate[] */
typedef fmi3Component fmi3InstantiateTYPE(fmi3String  instanceName,
                                          fmi3Type    fmuType,
                                          fmi3String  fmuGUID,
                                          fmi3String  fmuResourceLocation,
                                          const fmi3CallbackFunctions* functions,
                                          fmi3Boolean visible,
                                          fmi3Boolean loggingOn);
/* end::Instantiate[] */

/* tag::FreeInstance[] */
typedef void fmi3FreeInstanceTYPE(fmi3Component c);
/* end::FreeInstance[] */

/* Enter and exit initialization mode, terminate and reset */
/* tag::SetupExperiment[] */
typedef fmi3Status fmi3SetupExperimentTYPE(fmi3Component c,
                                           fmi3Boolean toleranceDefined,
                                           fmi3Float64 tolerance,
                                           fmi3Float64 startTime,
                                           fmi3Boolean stopTimeDefined,
                                           fmi3Float64 stopTime);
/* end::SetupExperiment[] */

/* tag::EnterInitializationMode[] */
typedef fmi3Status fmi3EnterInitializationModeTYPE(fmi3Component c);
/* end::EnterInitializationMode[] */

/* tag::ExitInitializationMode[] */
typedef fmi3Status fmi3ExitInitializationModeTYPE(fmi3Component c);
/* end::ExitInitializationMode[] */

/* tag::Terminate[] */
typedef fmi3Status fmi3TerminateTYPE(fmi3Component c);
/* end::Terminate[] */

/* tag::Reset[] */
typedef fmi3Status fmi3ResetTYPE(fmi3Component c);
/* end::Reset[] */

/* Getting and setting variable values */
/* tag::Getters[] */
typedef fmi3Status fmi3GetFloat32TYPE(fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      fmi3Float32 value[], size_t nValues);

typedef fmi3Status fmi3GetFloat64TYPE(fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      fmi3Float64 value[], size_t nValues);

typedef fmi3Status fmi3GetInt8TYPE   (fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      fmi3Int8 value[], size_t nValues);

typedef fmi3Status fmi3GetUInt8TYPE  (fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      fmi3UInt8 value[], size_t nValues);

typedef fmi3Status fmi3GetInt16TYPE  (fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      fmi3Int16 value[], size_t nValues);

typedef fmi3Status fmi3GetUInt16TYPE (fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      fmi3UInt16 value[], size_t nValues);

typedef fmi3Status fmi3GetInt32TYPE  (fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      fmi3Int32 value[], size_t nValues);

typedef fmi3Status fmi3GetUInt32TYPE (fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      fmi3UInt32 value[], size_t nValues);

typedef fmi3Status fmi3GetInt64TYPE  (fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      fmi3Int64 value[], size_t nValues);

typedef fmi3Status fmi3GetUInt64TYPE (fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      fmi3UInt64 value[], size_t nValues);

typedef fmi3Status fmi3GetBooleanTYPE(fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      fmi3Boolean value[], size_t nValues);

typedef fmi3Status fmi3GetStringTYPE (fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      fmi3String value[], size_t nValues);

typedef fmi3Status fmi3GetBinaryTYPE (fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      size_t size[], fmi3Binary value[], size_t nValues);
/* end::Getters[] */

/* tag::Setters[] */
typedef fmi3Status fmi3SetFloat32TYPE(fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      const fmi3Float32 value[], size_t nValues);

typedef fmi3Status fmi3SetFloat64TYPE(fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      const fmi3Float64 value[], size_t nValues);

typedef fmi3Status fmi3SetInt8TYPE   (fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      const fmi3Int8 value[], size_t nValues);

typedef fmi3Status fmi3SetUInt8TYPE  (fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      const fmi3UInt8 value[], size_t nValues);

typedef fmi3Status fmi3SetInt16TYPE  (fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      const fmi3Int16 value[], size_t nValues);

typedef fmi3Status fmi3SetUInt16TYPE (fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      const fmi3UInt16 value[], size_t nValues);

typedef fmi3Status fmi3SetInt32TYPE  (fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      const fmi3Int32 value[], size_t nValues);

typedef fmi3Status fmi3SetUInt32TYPE (fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      const fmi3UInt32 value[], size_t nValues);

typedef fmi3Status fmi3SetInt64TYPE  (fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      const fmi3Int64 value[], size_t nValues);

typedef fmi3Status fmi3SetUInt64TYPE (fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      const fmi3UInt64 value[], size_t nValues);

typedef fmi3Status fmi3SetBooleanTYPE(fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      const fmi3Boolean value[], size_t nValues);

typedef fmi3Status fmi3SetStringTYPE (fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      const fmi3String value[], size_t nValues);

typedef fmi3Status fmi3SetBinaryTYPE (fmi3Component c,
                                      const fmi3ValueReference vr[], size_t nvr,
                                      const size_t size[], const fmi3Binary value[], size_t nValues);
/* end::Setters[] */

/* Getting Variable Dependency Information */

/* tag::GetNumberOfVariableDependencies[] */
typedef fmi3Status fmi3GetNumberOfVariableDependenciesTYPE(fmi3Component c,
                                                           fmi3ValueReference vr,
                                                           size_t* nDependencies);
/* end::GetNumberOfVariableDependencies[] */

/* tag::GetVariableDependencies[] */
typedef fmi3Status fmi3GetVariableDependenciesTYPE(fmi3Component c,
                                                   fmi3ValueReference vrDependent,
                                                   size_t elementIndexDependent[],
                                                   fmi3ValueReference vrIndependent[],
                                                   size_t elementIndexIndependent[],
                                                   fmi3DependencyKind dependencyType[],
                                                   size_t nDependencies);
/* end::GetVariableDependencies[] */

/* Getting and setting the internal FMU state */

/* tag::GetSetFreeFMUstate[] */
typedef fmi3Status fmi3GetFMUstateTYPE (fmi3Component c, fmi3FMUstate* FMUstate);
typedef fmi3Status fmi3SetFMUstateTYPE (fmi3Component c, fmi3FMUstate  FMUstate);
typedef fmi3Status fmi3FreeFMUstateTYPE(fmi3Component c, fmi3FMUstate* FMUstate);
/* end::GetSetFreeFMUstate[] */

/* tag::SerializedFMUstate[] */
typedef fmi3Status fmi3SerializedFMUstateSizeTYPE(fmi3Component c,
                                                  fmi3FMUstate  FMUstate,
                                                  size_t* size);

typedef fmi3Status fmi3SerializeFMUstateTYPE     (fmi3Component c,
                                                  fmi3FMUstate  FMUstate,
                                                  fmi3Byte serializedState[],
                                                  size_t size);

typedef fmi3Status fmi3DeSerializeFMUstateTYPE   (fmi3Component c,
                                                  const fmi3Byte serializedState[],
                                                  size_t size,
                                                  fmi3FMUstate* FMUstate);
/* end::SerializedFMUstate[] */

/* Getting partial derivatives */

/* tag::GetDirectionalDerivative[] */
typedef fmi3Status fmi3GetDirectionalDerivativeTYPE(fmi3Component c,
                                                    const fmi3ValueReference vrUnknown[],
                                                    size_t nUnknown,
                                                    const fmi3ValueReference vrKnown[],
                                                    size_t nKnown,
                                                    const fmi3Float64 dvKnown[],
                                                    size_t nDvKnown,
                                                    fmi3Float64 dvUnknown[],
                                                    size_t nDvUnknown);
/* end::GetDirectionalDerivative[] */

/***************************************************
Types for Functions for FMI3 for Model Exchange
****************************************************/

/* Enter and exit the different modes */

/* tag::EnterEventMode[] */
typedef fmi3Status fmi3EnterEventModeTYPE(fmi3Component c);
/* end::EnterEventMode[] */

/* tag::NewDiscreteStates[] */
typedef fmi3Status fmi3NewDiscreteStatesTYPE(fmi3Component c,
                                             fmi3EventInfo* fmi3eventInfo);
/* end::NewDiscreteStates[] */

/* tag::EnterContinuousTimeMode[] */
typedef fmi3Status fmi3EnterContinuousTimeModeTYPE(fmi3Component c);
/* end::EnterContinuousTimeMode[] */

/* tag::CompletedIntegratorStep[] */
typedef fmi3Status fmi3CompletedIntegratorStepTYPE(fmi3Component c,
                                                   fmi3Boolean noSetFMUStatePriorToCurrentPoint,
                                                   fmi3Boolean* enterEventMode,
                                                   fmi3Boolean* terminateSimulation);
/* end::CompletedIntegratorStep[] */

/* Providing independent variables and re-initialization of caching */

/* tag::SetTime[] */
typedef fmi3Status fmi3SetTimeTYPE(fmi3Component c, fmi3Float64 time);
/* end::SetTime[] */

/* tag::SetContinuousStates[] */
typedef fmi3Status fmi3SetContinuousStatesTYPE(fmi3Component c,
                                               const fmi3Float64 x[],
                                               size_t nx);
/* end::SetContinuousStates[] */

/* Evaluation of the model equations */

/* tag::GetDerivatives[] */
typedef fmi3Status fmi3GetDerivativesTYPE(fmi3Component c,
                                          fmi3Float64 derivatives[],
                                          size_t nx);
/* end::GetDerivatives[] */

/* tag::GetEventIndicators[] */
typedef fmi3Status fmi3GetEventIndicatorsTYPE(fmi3Component c,
                                              fmi3Float64 eventIndicators[],
                                              size_t ni);
/* end::GetEventIndicators[] */

/* tag::GetContinuousStates[] */
typedef fmi3Status fmi3GetContinuousStatesTYPE(fmi3Component c, fmi3Float64 x[], size_t nx);
/* end::GetContinuousStates[] */

/* tag::GetNominalsOfContinuousStates[] */
typedef fmi3Status fmi3GetNominalsOfContinuousStatesTYPE(fmi3Component c,
                                                         fmi3Float64 x_nominal[],
                                                         size_t nx);
/* end::GetNominalsOfContinuousStates[] */

/* tag::GetNumberOfEventIndicators[] */
typedef fmi3Status fmi3GetNumberOfEventIndicatorsTYPE(fmi3Component c, size_t* nz);
/* end::GetNumberOfEventIndicators[] */

/* tag::GetNumberOfContinuousStates[] */
typedef fmi3Status fmi3GetNumberOfContinuousStatesTYPE(fmi3Component c, size_t* nx);
/* end::GetNumberOfContinuousStates[] */

/***************************************************
Types for Functions for FMI3 for Co-Simulation
****************************************************/

/* Simulating the slave */

/* tag::SetInputDerivatives[] */
typedef fmi3Status fmi3SetInputDerivativesTYPE(fmi3Component c,
                                               const fmi3ValueReference vr[],
                                               size_t nvr,
                                               const fmi3Int32 order[],
                                               const fmi3Float64 value[],
                                               size_t nValues);
/* end::SetInputDerivatives[] */

/* tag::GetOutputDerivatives[] */
typedef fmi3Status fmi3GetOutputDerivativesTYPE(fmi3Component c,
                                                const fmi3ValueReference vr[],
                                                size_t nvr,
                                                const fmi3Int32 order[],
                                                fmi3Float64 value[],
                                                size_t nValues);
/* end::GetOutputDerivatives[] */

/* tag::DoStep[] */
typedef fmi3Status fmi3DoStepTYPE(fmi3Component c,
                                  fmi3Float64 currentCommunicationPoint,
                                  fmi3Float64 communicationStepSize,
                                  fmi3Boolean noSetFMUStatePriorToCurrentPoint);
/* end::DoStep[] */

/* tag::CancelStep[] */
typedef fmi3Status fmi3CancelStepTYPE(fmi3Component c);
/* end::CancelStep[] */

/* Inquire slave status */

/* tag::GetDoStepPendingStatus[] */
typedef fmi3Status fmi3GetDoStepPendingStatusTYPE(fmi3Component c,
                                                  fmi3Status* status,
                                                  fmi3String* message);
/* end::GetDoStepPendingStatus[] */

/* tag::GetDoStepDiscardedStatus[] */
typedef fmi3Status fmi3GetDoStepDiscardedStatusTYPE(fmi3Component c,
                                                    fmi3Boolean* terminate,
                                                    fmi3Float64* lastSuccessfulTime);
/* end::GetDoStepDiscardedStatus[] */

#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif /* fmi3FunctionTypes_h */
