#ifndef fmi3FunctionTypes_h
#define fmi3FunctionTypes_h

#include "fmi3TypesPlatform.h"

/* This header file must be utilized when compiling an FMU or an FMI master.
   It declares data and function types for FMI 3.0-wg003.3

   Revisions:
   - Aug. 14, 2018: Fixed Input/Output Derivatives Signatures
   - Jul.  2, 2018: Changed fmi2 to fmi3 prefixes, merged binary type FCP-004
   - Jun. 13, 2018: Draft changes for WG-003 3.0 alignment, memory management callbacks (#379)
   - Nov. 16, 2017: Draft changes for WG-003 Arrays
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


   Copyright � 2011 MODELISAR consortium,
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
     fmi3Constant = 1,
     fmi3Fixed = 2,
     fmi3Tunable = 3,
     fmi3Discrete = 4,
     fmi3Dependent = 5
} fmi3DependencyKind;

typedef void  (*fmi3CallbackLogger)         (fmi3ComponentEnvironment environment, fmi3String instanceName, fmi3Status status, fmi3String category, fmi3String message, ...);
typedef void* (*fmi3CallbackAllocateMemory) (fmi3ComponentEnvironment environment, size_t nobj, size_t size);
typedef void  (*fmi3CallbackFreeMemory)     (fmi3ComponentEnvironment environment, void* obj);
typedef void  (*fmi3StepFinished)           (fmi3ComponentEnvironment environment, fmi3Status status);

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
typedef fmi3Status  fmi3SetDebugLoggingTYPE(fmi3Component component, fmi3Boolean loggingOn, size_t nCategories, const fmi3String categories[]);

/* Creation and destruction of FMU instances and setting debug status */
typedef fmi3Component fmi3InstantiateTYPE(fmi3String instanceName, fmi3Type fmiType, fmi3String guid, fmi3String resourceLocation, const fmi3CallbackFunctions* callbacks, fmi3Boolean visible, fmi3Boolean loggingOn);

typedef void fmi3FreeInstanceTYPE(fmi3Component component);

/* Enter and exit initialization mode, terminate and reset */
typedef fmi3Status fmi3SetupExperimentTYPE        (fmi3Component component, fmi3Boolean toleranceDefined, fmi3Real tolerance, fmi3Real startTime, fmi3Boolean stopTimeDefined, fmi3Real stopTime);
typedef fmi3Status fmi3EnterInitializationModeTYPE(fmi3Component component);
typedef fmi3Status fmi3ExitInitializationModeTYPE (fmi3Component component);
typedef fmi3Status fmi3TerminateTYPE              (fmi3Component component);
typedef fmi3Status fmi3ResetTYPE                  (fmi3Component component);

/* Getting and setting variable values */
typedef fmi3Status fmi3GetRealTYPE   (fmi3Component component, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Real    values[], size_t nValues);
typedef fmi3Status fmi3GetIntegerTYPE(fmi3Component component, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Integer values[], size_t nValues);
typedef fmi3Status fmi3GetBooleanTYPE(fmi3Component component, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3Boolean values[], size_t nValues);
typedef fmi3Status fmi3GetStringTYPE (fmi3Component component, const fmi3ValueReference valueReferences[], size_t nValueReferences, fmi3String  values[], size_t nValues);
typedef fmi3Status fmi3GetBinaryTYPE (fmi3Component component, const fmi3ValueReference valueReferences[], size_t nValueReferences, size_t valueSizes[], fmi3Binary values[], size_t nValues);

typedef fmi3Status fmi3SetRealTYPE   (fmi3Component component, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Real    values[], size_t nValues);
typedef fmi3Status fmi3SetIntegerTYPE(fmi3Component component, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Integer values[], size_t nValues);
typedef fmi3Status fmi3SetBooleanTYPE(fmi3Component component, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Boolean values[], size_t nValues);
typedef fmi3Status fmi3SetStringTYPE (fmi3Component component, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3String  values[], size_t nValues);
typedef fmi3Status fmi3SetBinaryTYPE (fmi3Component component, const fmi3ValueReference valueReferences[], size_t nValueReferences, const size_t valueSizes[], const fmi3Binary values[], size_t nValues);

/* Getting Variable Dependency Information */
typedef fmi3Status fmi3GetNumberOfVariableDependenciesTYPE(fmi3Component component, fmi3ValueReference valueReference, size_t* nDependencies);
typedef fmi3Status fmi3GetVariableDependenciesTYPE(fmi3Component component, fmi3ValueReference valueReferenceOfDependentVariable, size_t elementIndexOfDependentVariable[], fmi3ValueReference valueReferencesOfIndependentVariables[], size_t elementIndicesOfIndependentVariables[], fmi3DependencyKind dependencyKinds[], size_t nDependencies);

/* Getting and setting the internal FMU state */
typedef fmi3Status fmi3GetFMUstateTYPE           (fmi3Component component, fmi3FMUstate* state);
typedef fmi3Status fmi3SetFMUstateTYPE           (fmi3Component component, fmi3FMUstate  state);
typedef fmi3Status fmi3FreeFMUstateTYPE          (fmi3Component component, fmi3FMUstate* state);
typedef fmi3Status fmi3SerializedFMUstateSizeTYPE(fmi3Component component, fmi3FMUstate state, size_t* size);
typedef fmi3Status fmi3SerializeFMUstateTYPE     (fmi3Component component, fmi3FMUstate state, fmi3Byte serializedState[], size_t size);
typedef fmi3Status fmi3DeSerializeFMUstateTYPE   (fmi3Component component, const fmi3Byte serializedState[], size_t size, fmi3FMUstate* state);

/* Getting partial derivatives */
typedef fmi3Status fmi3GetDirectionalDerivativeTYPE(fmi3Component component, const fmi3ValueReference valueReferencesOfUnknowns[], size_t nUnknowns, const fmi3ValueReference valueReferencesOfKnowns[], size_t nKnowns, const fmi3Real derivativesOfKnowns[], size_t nDerivativesOfKnowns, fmi3Real derivativesOfUnknowns[], size_t nDerivativesOfUnknowns);

/***************************************************
Types for Functions for FMI3 for Model Exchange
****************************************************/

/* Enter and exit the different modes */
typedef fmi3Status fmi3EnterEventModeTYPE         (fmi3Component component);
typedef fmi3Status fmi3NewDiscreteStatesTYPE      (fmi3Component component, fmi3EventInfo* fmi2eventInfo);
typedef fmi3Status fmi3EnterContinuousTimeModeTYPE(fmi3Component component);
typedef fmi3Status fmi3CompletedIntegratorStepTYPE(fmi3Component component, fmi3Boolean noSetFMUStatePriorToCurrentPoint, fmi3Boolean* enterEventMode, fmi3Boolean* terminateSimulation);

/* Providing independent variables and re-initialization of caching */
typedef fmi3Status fmi3SetTimeTYPE            (fmi3Component component, fmi3Real time);
typedef fmi3Status fmi3SetContinuousStatesTYPE(fmi3Component component, const fmi3Real x[], size_t nx);

/* Evaluation of the model equations */
typedef fmi3Status fmi3GetDerivativesTYPE               (fmi3Component component, fmi3Real dx[], size_t nx);
typedef fmi3Status fmi3GetEventIndicatorsTYPE           (fmi3Component component, fmi3Real  z[], size_t nz);
typedef fmi3Status fmi3GetContinuousStatesTYPE          (fmi3Component component, fmi3Real  x[], size_t nx);
typedef fmi3Status fmi3GetNominalsOfContinuousStatesTYPE(fmi3Component component, fmi3Real nominals[], size_t nx);

typedef fmi3Status fmi3GetNumberOfEventIndicatorsTYPE   (fmi3Component component, size_t* nz);
typedef fmi3Status fmi3GetNumberOfContinuousStatesTYPE  (fmi3Component component, size_t* nx);

/***************************************************
Types for Functions for FMI3 for Co-Simulation
****************************************************/

/* Simulating the slave */
typedef fmi3Status fmi3SetRealInputDerivativesTYPE (fmi3Component component, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Integer order[], const fmi3Real values[], size_t nValues);
typedef fmi3Status fmi3GetRealOutputDerivativesTYPE(fmi3Component component, const fmi3ValueReference valueReferences[], size_t nValueReferences, const fmi3Integer order[], fmi3Real values[], size_t nValues);

typedef fmi3Status fmi3DoStepTYPE     (fmi3Component component, fmi3Real currentCommunicationPoint, fmi3Real communicationStepSize, fmi3Boolean noSetFMUStatePriorToCurrentPoint);
typedef fmi3Status fmi3CancelStepTYPE (fmi3Component component);

/* Inquire slave status */
typedef fmi3Status fmi3GetStatusTYPE       (fmi3Component component, const fmi3StatusKind kind, fmi3Status*  value);
typedef fmi3Status fmi3GetRealStatusTYPE   (fmi3Component component, const fmi3StatusKind kind, fmi3Real*    value);
typedef fmi3Status fmi3GetIntegerStatusTYPE(fmi3Component component, const fmi3StatusKind kind, fmi3Integer* value);
typedef fmi3Status fmi3GetBooleanStatusTYPE(fmi3Component component, const fmi3StatusKind kind, fmi3Boolean* value);
typedef fmi3Status fmi3GetStringStatusTYPE (fmi3Component component, const fmi3StatusKind kind, fmi3String*  value);

#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif /* fmi3FunctionTypes_h */
