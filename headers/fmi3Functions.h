#ifndef fmi3Functions_h
#define fmi3Functions_h

/* This header file must be utilized when compiling a FMU.
   It defines all functions of the
         FMI 3.0-wg003.3 Model Exchange and Co-Simulation Interface.

   In order to have unique function names even if several FMUs
   are compiled together (e.g. for embedded systems), every "real" function name
   is constructed by prepending the function name by "FMI3_FUNCTION_PREFIX".
   Therefore, the typical usage is:

      #define FMI3_FUNCTION_PREFIX MyModel_
      #include "fmi3Functions.h"

   As a result, a function that is defined as "fmi3GetDerivatives" in this header file,
   is actually getting the name "MyModel_fmi3GetDerivatives".

   This only holds if the FMU is shipped in C source code, or is compiled in a
   static link library. For FMUs compiled in a DLL/sharedObject, the "actual" function
   names are used and "FMI3_FUNCTION_PREFIX" must not be defined.

   Revisions:
   - Jul.  2, 2018: Changed fmi2 to fmi3 prefixes, merged binary type FCP-004
   - Jun. 13, 2018: Draft changes for WG-003 3.0 alignment, memory management callbacks (#379)
   - Nov. 16, 2017: Draft changes for WG-003 Arrays
   - Apr.  9, 2014: all prefixes "fmi" renamed to "fmi2" (decision from April 8)
   - Mar. 26, 2014: FMI_Export set to empty value if FMI_Export and FMI_FUNCTION_PREFIX
                    are not defined (#173)
   - Oct. 11, 2013: Functions of ModelExchange and CoSimulation merged:
                      fmiInstantiateModel , fmiInstantiateSlave  -> fmiInstantiate
                      fmiFreeModelInstance, fmiFreeSlaveInstance -> fmiFreeInstance
                      fmiEnterModelInitializationMode, fmiEnterSlaveInitializationMode -> fmiEnterInitializationMode
                      fmiExitModelInitializationMode , fmiExitSlaveInitializationMode  -> fmiExitInitializationMode
                      fmiTerminateModel, fmiTerminateSlave  -> fmiTerminate
                      fmiResetSlave -> fmiReset (now also for ModelExchange and not only for CoSimulation)
                    Functions renamed:
                      fmiUpdateDiscreteStates -> fmiNewDiscreteStates
   - June 13, 2013: Functions removed:
                       fmiInitializeModel
                       fmiEventUpdate
                       fmiCompletedEventIteration
                       fmiInitializeSlave
                    Functions added:
                       fmiEnterModelInitializationMode
                       fmiExitModelInitializationMode
                       fmiEnterEventMode
                       fmiUpdateDiscreteStates
                       fmiEnterContinuousTimeMode
                       fmiEnterSlaveInitializationMode;
                       fmiExitSlaveInitializationMode;
   - Feb. 17, 2013: Portability improvements:
                       o DllExport changed to FMI_Export
                       o FUNCTION_PREFIX changed to FMI_FUNCTION_PREFIX
                       o Allow undefined FMI_FUNCTION_PREFIX (meaning no prefix is used)
                    Changed function name "fmiTerminate" to "fmiTerminateModel" (due to #113)
                    Changed function name "fmiGetNominalContinuousState" to
                                          "fmiGetNominalsOfContinuousStates"
                    Removed fmiGetStateValueReferences.
   - Nov. 14, 2011: Adapted to FMI 2.0:
                       o Split into two files (fmiFunctions.h, fmiTypes.h) in order
                         that code that dynamically loads an FMU can directly
                         utilize the header files).
                       o Added C++ encapsulation of C-part, in order that the header
                         file can be directly utilized in C++ code.
                       o fmiCallbackFunctions is passed as pointer to fmiInstantiateXXX
                       o stepFinished within fmiCallbackFunctions has as first
                         argument "fmiComponentEnvironment" and not "fmiComponent".
                       o New functions to get and set the complete FMU state
                         and to compute partial derivatives.
   - Nov.  4, 2010: Adapted to specification text:
                       o fmiGetModelTypesPlatform renamed to fmiGetTypesPlatform
                       o fmiInstantiateSlave: Argument GUID     replaced by fmuGUID
                                              Argument mimetype replaced by mimeType
                       o tabs replaced by spaces
   - Oct. 16, 2010: Functions for FMI for Co-simulation added
   - Jan. 20, 2010: stateValueReferencesChanged added to struct fmiEventInfo (ticket #27)
                    (by M. Otter, DLR)
                    Added WIN32 pragma to define the struct layout (ticket #34)
                    (by J. Mauss, QTronic)
   - Jan.  4, 2010: Removed argument intermediateResults from fmiInitialize
                    Renamed macro fmiGetModelFunctionsVersion to fmiGetVersion
                    Renamed macro fmiModelFunctionsVersion to fmiVersion
                    Replaced fmiModel by fmiComponent in decl of fmiInstantiateModel
                    (by J. Mauss, QTronic)
   - Dec. 17, 2009: Changed extension "me" to "fmi" (by Martin Otter, DLR).
   - Dez. 14, 2009: Added eventInfo to meInitialize and added
                    meGetNominalContinuousStates (by Martin Otter, DLR)
   - Sept. 9, 2009: Added DllExport (according to Peter Nilsson's suggestion)
                    (by A. Junghanns, QTronic)
   - Sept. 9, 2009: Changes according to FMI-meeting on July 21:
                    meInquireModelTypesVersion     -> meGetModelTypesPlatform
                    meInquireModelFunctionsVersion -> meGetModelFunctionsVersion
                    meSetStates                    -> meSetContinuousStates
                    meGetStates                    -> meGetContinuousStates
                    removal of meInitializeModelClass
                    removal of meGetTime
                    change of arguments of meInstantiateModel
                    change of arguments of meCompletedIntegratorStep
                    (by Martin Otter, DLR):
   - July 19, 2009: Added "me" as prefix to file names (by Martin Otter, DLR).
   - March 2, 2009: Changed function definitions according to the last design
                    meeting with additional improvements (by Martin Otter, DLR).
   - Dec. 3 , 2008: First version by Martin Otter (DLR) and Hans Olsson (Dynasim).

   Copyright © 2008-2011 MODELISAR consortium,
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

#include "fmi3TypesPlatform.h"
#include "fmi3FunctionTypes.h"
#include <stdlib.h>


/*
  Export FMI3 API functions on Windows and under GCC.
  If custom linking is desired then the FMI3_Export must be
  defined before including this file. For instance,
  it may be set to __declspec(dllimport).
*/
#if !defined(FMI3_Export)
  #if !defined(FMI3_FUNCTION_PREFIX)
    #if defined _WIN32 || defined __CYGWIN__
     /* Note: both gcc & MSVC on Windows support this syntax. */
        #define FMI3_Export __declspec(dllexport)
    #else
      #if __GNUC__ >= 4
        #define FMI3_Export __attribute__ ((visibility ("default")))
      #else
        #define FMI3_Export
      #endif
    #endif
  #else
    #define FMI3_Export
  #endif
#endif

/* Macros to construct the real function name
   (prepend function name by FMI3_FUNCTION_PREFIX) */
#if defined(FMI3_FUNCTION_PREFIX)
  #define fmi3Paste(a,b)     a ## b
  #define fmi3PasteB(a,b)    fmi3Paste(a,b)
  #define fmi3FullName(name) fmi3PasteB(FMI3_FUNCTION_PREFIX, name)
#else
  #define fmi3FullName(name) name
#endif

/***************************************************
Common Functions
****************************************************/
#define fmi3GetTypesPlatform         fmi3FullName(fmi3GetTypesPlatform)
#define fmi3GetVersion               fmi3FullName(fmi3GetVersion)
#define fmi3SetDebugLogging          fmi3FullName(fmi3SetDebugLogging)
#define fmi3Instantiate              fmi3FullName(fmi3Instantiate)
#define fmi3FreeInstance             fmi3FullName(fmi3FreeInstance)
#define fmi3SetupExperiment          fmi3FullName(fmi3SetupExperiment)
#define fmi3EnterInitializationMode  fmi3FullName(fmi3EnterInitializationMode)
#define fmi3ExitInitializationMode   fmi3FullName(fmi3ExitInitializationMode)
#define fmi3Terminate                fmi3FullName(fmi3Terminate)
#define fmi3Reset                    fmi3FullName(fmi3Reset)
#define fmi3GetReal                  fmi3FullName(fmi3GetReal)
#define fmi3GetInteger               fmi3FullName(fmi3GetInteger)
#define fmi3GetBoolean               fmi3FullName(fmi3GetBoolean)
#define fmi3GetString                fmi3FullName(fmi3GetString)
#define fmi3GetBinary                fmi3FullName(fmi3GetBinary)
#define fmi3SetReal                  fmi3FullName(fmi3SetReal)
#define fmi3SetInteger               fmi3FullName(fmi3SetInteger)
#define fmi3SetBoolean               fmi3FullName(fmi3SetBoolean)
#define fmi3SetString                fmi3FullName(fmi3SetString)
#define fmi3SetBinary                fmi3FullName(fmi3SetBinary)
#define fmi3GetNumberOfVariableDependencies fmi3FullName(fmi3GetNumberOfVariableDependencies)
#define fmi3GetVariableDependencies  fmi3FullName(fmi3GetVariableDependencies)
#define fmi3GetFMUstate              fmi3FullName(fmi3GetFMUstate)
#define fmi3SetFMUstate              fmi3FullName(fmi3SetFMUstate)
#define fmi3FreeFMUstate             fmi3FullName(fmi3FreeFMUstate)
#define fmi3SerializedFMUstateSize   fmi3FullName(fmi3SerializedFMUstateSize)
#define fmi3SerializeFMUstate        fmi3FullName(fmi3SerializeFMUstate)
#define fmi3DeSerializeFMUstate      fmi3FullName(fmi3DeSerializeFMUstate)
#define fmi3GetDirectionalDerivative fmi3FullName(fmi3GetDirectionalDerivative)


/***************************************************
Functions for FMI3 for Model Exchange
****************************************************/
#define fmi3EnterEventMode                fmi3FullName(fmi3EnterEventMode)
#define fmi3NewDiscreteStates             fmi3FullName(fmi3NewDiscreteStates)
#define fmi3EnterContinuousTimeMode       fmi3FullName(fmi3EnterContinuousTimeMode)
#define fmi3CompletedIntegratorStep       fmi3FullName(fmi3CompletedIntegratorStep)
#define fmi3SetTime                       fmi3FullName(fmi3SetTime)
#define fmi3SetContinuousStates           fmi3FullName(fmi3SetContinuousStates)
#define fmi3GetDerivatives                fmi3FullName(fmi3GetDerivatives)
#define fmi3GetEventIndicators            fmi3FullName(fmi3GetEventIndicators)
#define fmi3GetContinuousStates           fmi3FullName(fmi3GetContinuousStates)
#define fmi3GetNominalsOfContinuousStates fmi3FullName(fmi3GetNominalsOfContinuousStates)
#define fmi3GetNumberOfEventIndicators            fmi3FullName(fmi3GetNumberOfEventIndicators)
#define fmi3GetNumberOfContinuousStates           fmi3FullName(fmi3GetNumberOfContinuousStates)


/***************************************************
Functions for FMI3 for Co-Simulation
****************************************************/
#define fmi3SetRealInputDerivatives      fmi3FullName(fmi3SetRealInputDerivatives)
#define fmi3GetRealOutputDerivatives     fmi3FullName(fmi3GetRealOutputDerivatives)
#define fmi3DoStep                       fmi3FullName(fmi3DoStep)
#define fmi3CancelStep                   fmi3FullName(fmi3CancelStep)
#define fmi3GetStatus                    fmi3FullName(fmi3GetStatus)
#define fmi3GetRealStatus                fmi3FullName(fmi3GetRealStatus)
#define fmi3GetIntegerStatus             fmi3FullName(fmi3GetIntegerStatus)
#define fmi3GetBooleanStatus             fmi3FullName(fmi3GetBooleanStatus)
#define fmi3GetStringStatus              fmi3FullName(fmi3GetStringStatus)

/* Version number */
#define fmi3Version "3.0-wg003.3"


/***************************************************
Common Functions
****************************************************/

/* Inquire version numbers of header files */
   FMI3_Export fmi3GetTypesPlatformTYPE fmi3GetTypesPlatform;
   FMI3_Export fmi3GetVersionTYPE       fmi3GetVersion;
   FMI3_Export fmi3SetDebugLoggingTYPE  fmi3SetDebugLogging;

/* Creation and destruction of FMU instances */
   FMI3_Export fmi3InstantiateTYPE  fmi3Instantiate;
   FMI3_Export fmi3FreeInstanceTYPE fmi3FreeInstance;

/* Enter and exit initialization mode, terminate and reset */
   FMI3_Export fmi3SetupExperimentTYPE         fmi3SetupExperiment;
   FMI3_Export fmi3EnterInitializationModeTYPE fmi3EnterInitializationMode;
   FMI3_Export fmi3ExitInitializationModeTYPE  fmi3ExitInitializationMode;
   FMI3_Export fmi3TerminateTYPE               fmi3Terminate;
   FMI3_Export fmi3ResetTYPE                   fmi3Reset;

/* Getting and setting variables values */
   FMI3_Export fmi3GetRealTYPE    fmi3GetReal;
   FMI3_Export fmi3GetIntegerTYPE fmi3GetInteger;
   FMI3_Export fmi3GetBooleanTYPE fmi3GetBoolean;
   FMI3_Export fmi3GetStringTYPE  fmi3GetString;
   FMI3_Export fmi3GetBinaryTYPE  fmi3GetBinary;

   FMI3_Export fmi3SetRealTYPE    fmi3SetReal;
   FMI3_Export fmi3SetIntegerTYPE fmi3SetInteger;
   FMI3_Export fmi3SetBooleanTYPE fmi3SetBoolean;
   FMI3_Export fmi3SetStringTYPE  fmi3SetString;
   FMI3_Export fmi3SetBinaryTYPE  fmi3SetBinary;

/* Getting Variable Dependency Information */
   FMI3_Export fmi3GetNumberOfVariableDependenciesTYPE fmi3GetNumberOfVariableDependencies;
   FMI3_Export fmi3GetVariableDependenciesTYPE fmi3GetVariableDependencies;

/* Getting and setting the internal FMU state */
   FMI3_Export fmi3GetFMUstateTYPE            fmi3GetFMUstate;
   FMI3_Export fmi3SetFMUstateTYPE            fmi3SetFMUstate;
   FMI3_Export fmi3FreeFMUstateTYPE           fmi3FreeFMUstate;
   FMI3_Export fmi3SerializedFMUstateSizeTYPE fmi3SerializedFMUstateSize;
   FMI3_Export fmi3SerializeFMUstateTYPE      fmi3SerializeFMUstate;
   FMI3_Export fmi3DeSerializeFMUstateTYPE    fmi3DeSerializeFMUstate;

/* Getting partial derivatives */
   FMI3_Export fmi3GetDirectionalDerivativeTYPE fmi3GetDirectionalDerivative;


/***************************************************
Functions for FMI3 for Model Exchange
****************************************************/

/* Enter and exit the different modes */
   FMI3_Export fmi3EnterEventModeTYPE               fmi3EnterEventMode;
   FMI3_Export fmi3NewDiscreteStatesTYPE            fmi3NewDiscreteStates;
   FMI3_Export fmi3EnterContinuousTimeModeTYPE      fmi3EnterContinuousTimeMode;
   FMI3_Export fmi3CompletedIntegratorStepTYPE      fmi3CompletedIntegratorStep;

/* Providing independent variables and re-initialization of caching */
   FMI3_Export fmi3SetTimeTYPE             fmi3SetTime;
   FMI3_Export fmi3SetContinuousStatesTYPE fmi3SetContinuousStates;

/* Evaluation of the model equations */
   FMI3_Export fmi3GetDerivativesTYPE                fmi3GetDerivatives;
   FMI3_Export fmi3GetEventIndicatorsTYPE            fmi3GetEventIndicators;
   FMI3_Export fmi3GetContinuousStatesTYPE           fmi3GetContinuousStates;
   FMI3_Export fmi3GetNominalsOfContinuousStatesTYPE fmi3GetNominalsOfContinuousStates;
   FMI3_Export fmi3GetNumberOfEventIndicatorsTYPE    fmi3GetNumberOfEventIndicators;
   FMI3_Export fmi3GetNumberOfContinuousStatesTYPE   fmi3GetNumberOfContinuousStates;


/***************************************************
Functions for FMI3 for Co-Simulation
****************************************************/

/* Simulating the slave */
   FMI3_Export fmi3SetRealInputDerivativesTYPE  fmi3SetRealInputDerivatives;
   FMI3_Export fmi3GetRealOutputDerivativesTYPE fmi3GetRealOutputDerivatives;

   FMI3_Export fmi3DoStepTYPE     fmi3DoStep;
   FMI3_Export fmi3CancelStepTYPE fmi3CancelStep;

/* Inquire slave status */
   FMI3_Export fmi3GetStatusTYPE        fmi3GetStatus;
   FMI3_Export fmi3GetRealStatusTYPE    fmi3GetRealStatus;
   FMI3_Export fmi3GetIntegerStatusTYPE fmi3GetIntegerStatus;
   FMI3_Export fmi3GetBooleanStatusTYPE fmi3GetBooleanStatus;
   FMI3_Export fmi3GetStringStatusTYPE  fmi3GetStringStatus;

#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif /* fmi3Functions_h */
