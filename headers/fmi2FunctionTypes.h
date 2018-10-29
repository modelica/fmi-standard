#ifndef fmi2FunctionTypes_h
#define fmi2FunctionTypes_h

#include "fmi2TypesPlatform.h"

/* This header file must be utilized when compiling an FMU or an FMI master.
   It declares data and function types for FMI 2.0

   Revisions:
   - Sep.  6, 2018: Parameter names added to function prototypes
   - Apr.  9, 2014: all prefixes "fmi" renamed to "fmi2" (decision from April 8)
   - Apr.  3, 2014: Added #include <stddef.h> for size_t definition
   - Mar. 27, 2014: Added #include "fmiTypesPlatform.h" (#179)
   - Mar. 26, 2014: Introduced function argument "void" for the functions (#171)
                      fmiGetTypesPlatformTYPE and fmiGetVersionTYPE
   - Oct. 11, 2013: Functions of ModelExchange and CoSimulation merged:
                      fmiInstantiateModelTYPE , fmiInstantiateSlaveTYPE  -> fmiInstantiateTYPE
                      fmiFreeModelInstanceTYPE, fmiFreeSlaveInstanceTYPE -> fmiFreeInstanceTYPE
                      fmiEnterModelInitializationModeTYPE, fmiEnterSlaveInitializationModeTYPE -> fmiEnterInitializationModeTYPE
                      fmiExitModelInitializationModeTYPE , fmiExitSlaveInitializationModeTYPE  -> fmiExitInitializationModeTYPE
                      fmiTerminateModelTYPE , fmiTerminateSlaveTYPE  -> fmiTerminate
                      fmiResetSlave -> fmiReset (now also for ModelExchange and not only for CoSimulation)
                    Functions renamed
                      fmiUpdateDiscreteStatesTYPE -> fmiNewDiscreteStatesTYPE
                    Renamed elements of the enumeration fmiEventInfo
                      upcomingTimeEvent             -> nextEventTimeDefined // due to generic naming scheme: varDefined + var
                      newUpdateDiscreteStatesNeeded -> newDiscreteStatesNeeded;
   - June 13, 2013: Changed type fmiEventInfo
                    Functions removed:
                       fmiInitializeModelTYPE
                       fmiEventUpdateTYPE
                       fmiCompletedEventIterationTYPE
                       fmiInitializeSlaveTYPE
                    Functions added:
                       fmiEnterModelInitializationModeTYPE
                       fmiExitModelInitializationModeTYPE
                       fmiEnterEventModeTYPE
                       fmiUpdateDiscreteStatesTYPE
                       fmiEnterContinuousTimeModeTYPE
                       fmiEnterSlaveInitializationModeTYPE;
                       fmiExitSlaveInitializationModeTYPE;
   - Feb. 17, 2013: Added third argument to fmiCompletedIntegratorStepTYPE
                    Changed function name "fmiTerminateType" to "fmiTerminateModelType" (due to #113)
                    Changed function name "fmiGetNominalContinuousStateTYPE" to
                                          "fmiGetNominalsOfContinuousStatesTYPE"
                    Removed fmiGetStateValueReferencesTYPE.
   - Nov. 14, 2011: First public Version


   Copyright (C) 2011 MODELISAR consortium,
                 2012-2013 Modelica Association Project "FMI"
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
    fmi2OK,
    fmi2Warning,
    fmi2Discard,
    fmi2Error,
    fmi2Fatal,
    fmi2Pending
} fmi2Status;

typedef enum {
    fmi2ModelExchange,
    fmi2CoSimulation
} fmi2Type;

typedef enum {
    fmi2DoStepStatus,
    fmi2PendingStatus,
    fmi2LastSuccessfulTime,
    fmi2Terminated
} fmi2StatusKind;

typedef void      (*fmi2CallbackLogger)        (fmi2ComponentEnvironment componentEnvironment,
                                                fmi2String instanceName,
                                                fmi2Status status,
                                                fmi2String category,
                                                fmi2String message,
                                                ...);
typedef void*     (*fmi2CallbackAllocateMemory)(size_t nobj, size_t size);
typedef void      (*fmi2CallbackFreeMemory)    (void* obj);
typedef void      (*fmi2StepFinished)          (fmi2ComponentEnvironment componentEnvironment,
                                                fmi2Status status);

typedef struct {
   const fmi2CallbackLogger         logger;
   const fmi2CallbackAllocateMemory allocateMemory;
   const fmi2CallbackFreeMemory     freeMemory;
   const fmi2StepFinished           stepFinished;
   const fmi2ComponentEnvironment   componentEnvironment;
} fmi2CallbackFunctions;

typedef struct {
   fmi2Boolean newDiscreteStatesNeeded;
   fmi2Boolean terminateSimulation;
   fmi2Boolean nominalsOfContinuousStatesChanged;
   fmi2Boolean valuesOfContinuousStatesChanged;
   fmi2Boolean nextEventTimeDefined;
   fmi2Real    nextEventTime;
} fmi2EventInfo;


/* reset alignment policy to the one set before reading this file */
#if defined _MSC_VER || defined __GNUC__
#pragma pack(pop)
#endif


/* Define fmi2 function pointer types to simplify dynamic loading */

/***************************************************
Types for Common Functions
****************************************************/

/* Inquire version numbers of header files and setting logging status */
    typedef const char* fmi2GetTypesPlatformTYPE(void);
    typedef const char* fmi2GetVersionTYPE(void);
    typedef fmi2Status  fmi2SetDebugLoggingTYPE(fmi2Component c,
                                                fmi2Boolean loggingOn,
                                                size_t nCategories,
                                                const fmi2String categories[]);

/* Creation and destruction of FMU instances and setting debug status */
    typedef fmi2Component fmi2InstantiateTYPE(fmi2String instanceName,
                                              fmi2Type fmuType,
                                              fmi2String fmuGUID,
                                              fmi2String fmuResourceLocation,
                                              const fmi2CallbackFunctions* functions,
                                              fmi2Boolean visible,
                                              fmi2Boolean loggingOn);
   typedef void          fmi2FreeInstanceTYPE(fmi2Component c);

/* Enter and exit initialization mode, terminate and reset */
    typedef fmi2Status fmi2SetupExperimentTYPE       (fmi2Component c,
                                                      fmi2Boolean toleranceDefined,
                                                      fmi2Real tolerance,
                                                      fmi2Real startTime,
                                                      fmi2Boolean stopTimeDefined,
                                                      fmi2Real stopTime);
   typedef fmi2Status fmi2EnterInitializationModeTYPE(fmi2Component c);
   typedef fmi2Status fmi2ExitInitializationModeTYPE (fmi2Component c);
   typedef fmi2Status fmi2TerminateTYPE              (fmi2Component c);
   typedef fmi2Status fmi2ResetTYPE                  (fmi2Component c);

/* Getting and setting variable values */
   typedef fmi2Status fmi2GetRealTYPE   (fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Real    value[]);
   typedef fmi2Status fmi2GetIntegerTYPE(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[]);
   typedef fmi2Status fmi2GetBooleanTYPE(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[]);
   typedef fmi2Status fmi2GetStringTYPE (fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2String  value[]);

   typedef fmi2Status fmi2SetRealTYPE   (fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Real    value[]);
   typedef fmi2Status fmi2SetIntegerTYPE(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[]);
   typedef fmi2Status fmi2SetBooleanTYPE(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[]);
   typedef fmi2Status fmi2SetStringTYPE (fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2String  value[]);

/* Getting and setting the internal FMU state */
   typedef fmi2Status fmi2GetFMUstateTYPE           (fmi2Component c, fmi2FMUstate* FMUstate);
   typedef fmi2Status fmi2SetFMUstateTYPE           (fmi2Component c, fmi2FMUstate  FMUstate);
   typedef fmi2Status fmi2FreeFMUstateTYPE          (fmi2Component c, fmi2FMUstate* FMUstate);
   typedef fmi2Status fmi2SerializedFMUstateSizeTYPE(fmi2Component c, fmi2FMUstate  FMUstate, size_t* size);
   typedef fmi2Status fmi2SerializeFMUstateTYPE     (fmi2Component c, fmi2FMUstate  FMUstate, fmi2Byte[], size_t size);
   typedef fmi2Status fmi2DeSerializeFMUstateTYPE   (fmi2Component c, const fmi2Byte serializedState[], size_t size, fmi2FMUstate* FMUstate);

/* Getting partial derivatives */
    typedef fmi2Status fmi2GetDirectionalDerivativeTYPE(fmi2Component c,
                                                        const fmi2ValueReference vUnknown_ref[], size_t nUnknown,
                                                        const fmi2ValueReference vKnown_ref[],   size_t nKnown,
                                                        const fmi2Real dvKnown[],
                                                        fmi2Real dvUnknown[]);

/***************************************************
Types for Functions for FMI2 for Model Exchange
****************************************************/

/* Enter and exit the different modes */
   typedef fmi2Status fmi2EnterEventModeTYPE         (fmi2Component c);
   typedef fmi2Status fmi2NewDiscreteStatesTYPE      (fmi2Component c, fmi2EventInfo* fmi2eventInfo);
   typedef fmi2Status fmi2EnterContinuousTimeModeTYPE(fmi2Component c);
   typedef fmi2Status fmi2CompletedIntegratorStepTYPE(fmi2Component c,
                                                      fmi2Boolean   noSetFMUStatePriorToCurrentPoint,
                                                      fmi2Boolean*  enterEventMode,
                                                      fmi2Boolean*  terminateSimulation);

/* Providing independent variables and re-initialization of caching */
   typedef fmi2Status fmi2SetTimeTYPE            (fmi2Component c, fmi2Real time);
   typedef fmi2Status fmi2SetContinuousStatesTYPE(fmi2Component c, const fmi2Real x[], size_t nx);

/* Evaluation of the model equations */
   typedef fmi2Status fmi2GetDerivativesTYPE               (fmi2Component c, fmi2Real derivatives[],     size_t nx);
   typedef fmi2Status fmi2GetEventIndicatorsTYPE           (fmi2Component c, fmi2Real eventIndicators[], size_t ni);
   typedef fmi2Status fmi2GetContinuousStatesTYPE          (fmi2Component c, fmi2Real x[],               size_t nx);
   typedef fmi2Status fmi2GetNominalsOfContinuousStatesTYPE(fmi2Component c, fmi2Real x_nominal[],       size_t nx);


/***************************************************
Types for Functions for FMI2 for Co-Simulation
****************************************************/

/* Simulating the slave */
    typedef fmi2Status fmi2SetRealInputDerivativesTYPE (fmi2Component c,
                                                        const fmi2ValueReference vr[], size_t nvr,
                                                        const fmi2Integer order[],
                                                        const fmi2Real value[]);
    typedef fmi2Status fmi2GetRealOutputDerivativesTYPE(fmi2Component c,
                                                        const fmi2ValueReference vr[], size_t nvr,
                                                        const fmi2Integer order[],
                                                        fmi2Real value[]);
    typedef fmi2Status fmi2DoStepTYPE   (fmi2Component c,
                                         fmi2Real      currentCommunicationPoint,
                                         fmi2Real      communicationStepSize,
                                         fmi2Boolean   noSetFMUStatePriorToCurrentPoint);
   typedef fmi2Status fmi2CancelStepTYPE(fmi2Component c);

/* Inquire slave status */
   typedef fmi2Status fmi2GetStatusTYPE       (fmi2Component c, const fmi2StatusKind s, fmi2Status*  value);
   typedef fmi2Status fmi2GetRealStatusTYPE   (fmi2Component c, const fmi2StatusKind s, fmi2Real*    value);
   typedef fmi2Status fmi2GetIntegerStatusTYPE(fmi2Component c, const fmi2StatusKind s, fmi2Integer* value);
   typedef fmi2Status fmi2GetBooleanStatusTYPE(fmi2Component c, const fmi2StatusKind s, fmi2Boolean* value);
   typedef fmi2Status fmi2GetStringStatusTYPE (fmi2Component c, const fmi2StatusKind s, fmi2String*  value);


#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif /* fmi2FunctionTypes_h */
