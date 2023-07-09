#ifndef fmi3DirectMemory_h
#define fmi3DirectMemory_h

/*
This header file declares the optional direct memory access functions of FMI 3.1.
It must be used when compiling an FMU.

Copyright (C) 2008-2011 MODELISAR consortium,
              2012-2023 Modelica Association Project "FMI"
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

#include "fmi3Functions.h"

/***************************************************
Functions for Direct Memory Access
****************************************************/

typedef fmi3Status fmi3GetFloat32PointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Float32* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3GetFloat64PointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Float64* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3GetInt8PointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Int8* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3GetUInt8PointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt8* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3GetInt16PointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Int16* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3GetUInt16PointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt16* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3GetInt32PointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Int32* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3GetUInt32PointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt32* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3GetInt64PointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Int64* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3GetUInt64PointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt64* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3GetBooleanPointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Boolean* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3GetStringPointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3String valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3GetBinaryPointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    size_t valueSizes[],
    fmi3Binary valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3GetClockPointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Clock* valuePointers[]);

typedef fmi3Status fmi3SetFloat32PointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Float32* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3SetFloat64PointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Float64* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3SetInt8PointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Int8* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3SetUInt8PointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt8* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3SetInt16PointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Int16* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3SetUInt16PointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt16* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3SetInt32PointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Int32* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3SetUInt32PointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt32* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3SetInt64PointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Int64* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3SetUInt64PointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt64* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3SetBooleanPointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Boolean* valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3SetStringPointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3String valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3SetBinaryPointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const size_t valueSizes[],
    fmi3Binary valuePointers[],
    size_t nValues);

typedef fmi3Status fmi3SetClockPointerTYPE(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Clock* valuePointers[]);

#define fmi3GetFloat32Pointer fmi3FullName(fmi3GetFloat32Pointer)
#define fmi3GetFloat64Pointer fmi3FullName(fmi3GetFloat64Pointer)
#define fmi3GetInt8Pointer    fmi3FullName(fmi3GetInt8Pointer)
#define fmi3GetUInt8Pointer   fmi3FullName(fmi3GetUInt8Pointer)
#define fmi3GetInt16Pointer   fmi3FullName(fmi3GetInt16Pointer)
#define fmi3GetUInt16Pointer  fmi3FullName(fmi3GetUInt16Pointer)
#define fmi3GetInt32Pointer   fmi3FullName(fmi3GetInt32Pointer)
#define fmi3GetUInt32Pointer  fmi3FullName(fmi3GetUInt32Pointer)
#define fmi3GetInt64Pointer   fmi3FullName(fmi3GetInt64Pointer)
#define fmi3GetUInt64Pointer  fmi3FullName(fmi3GetUInt64Pointer)
#define fmi3GetBooleanPointer fmi3FullName(fmi3GetBooleanPointer)
#define fmi3GetStringPointer  fmi3FullName(fmi3GetStringPointer)
#define fmi3GetBinaryPointer  fmi3FullName(fmi3GetBinaryPointer)
#define fmi3GetClockPointer   fmi3FullName(fmi3GetClockPointer)
#define fmi3SetFloat32Pointer fmi3FullName(fmi3SetFloat32Pointer)
#define fmi3SetFloat64Pointer fmi3FullName(fmi3SetFloat64Pointer)
#define fmi3SetInt8Pointer    fmi3FullName(fmi3SetInt8Pointer)
#define fmi3SetUInt8Pointer   fmi3FullName(fmi3SetUInt8Pointer)
#define fmi3SetInt16Pointer   fmi3FullName(fmi3SetInt16Pointer)
#define fmi3SetUInt16Pointer  fmi3FullName(fmi3SetUInt16Pointer)
#define fmi3SetInt32Pointer   fmi3FullName(fmi3SetInt32Pointer)
#define fmi3SetUInt32Pointer  fmi3FullName(fmi3SetUInt32Pointer)
#define fmi3SetInt64Pointer   fmi3FullName(fmi3SetInt64Pointer)
#define fmi3SetUInt64Pointer  fmi3FullName(fmi3SetUInt64Pointer)
#define fmi3SetBooleanPointer fmi3FullName(fmi3SetBooleanPointer)
#define fmi3SetStringPointer  fmi3FullName(fmi3SetStringPointer)
#define fmi3SetBinaryPointer  fmi3FullName(fmi3SetBinaryPointer)
#define fmi3SetClockPointer   fmi3FullName(fmi3SetClockPointer)

FMI3_Export fmi3GetFloat32PointerTYPE fmi3GetFloat32Pointer;
FMI3_Export fmi3GetFloat64PointerTYPE fmi3GetFloat64Pointer;
FMI3_Export fmi3GetInt8PointerTYPE    fmi3GetInt8Pointer;
FMI3_Export fmi3GetUInt8PointerTYPE   fmi3GetUInt8Pointer;
FMI3_Export fmi3GetInt16PointerTYPE   fmi3GetInt16Pointer;
FMI3_Export fmi3GetUInt16PointerTYPE  fmi3GetUInt16Pointer;
FMI3_Export fmi3GetInt32PointerTYPE   fmi3GetInt32Pointer;
FMI3_Export fmi3GetUInt32PointerTYPE  fmi3GetUInt32Pointer;
FMI3_Export fmi3GetInt64PointerTYPE   fmi3GetInt64Pointer;
FMI3_Export fmi3GetUInt64PointerTYPE  fmi3GetUInt64Pointer;
FMI3_Export fmi3GetBooleanPointerTYPE fmi3GetBooleanPointer;
FMI3_Export fmi3GetStringPointerTYPE  fmi3GetStringPointer;
FMI3_Export fmi3GetBinaryPointerTYPE  fmi3GetBinaryPointer;
FMI3_Export fmi3GetClockPointerTYPE   fmi3GetClockPointer;
FMI3_Export fmi3SetFloat32PointerTYPE fmi3SetFloat32Pointer;
FMI3_Export fmi3SetFloat64PointerTYPE fmi3SetFloat64Pointer;
FMI3_Export fmi3SetInt8PointerTYPE    fmi3SetInt8Pointer;
FMI3_Export fmi3SetUInt8PointerTYPE   fmi3SetUInt8Pointer;
FMI3_Export fmi3SetInt16PointerTYPE   fmi3SetInt16Pointer;
FMI3_Export fmi3SetUInt16PointerTYPE  fmi3SetUInt16Pointer;
FMI3_Export fmi3SetInt32PointerTYPE   fmi3SetInt32Pointer;
FMI3_Export fmi3SetUInt32PointerTYPE  fmi3SetUInt32Pointer;
FMI3_Export fmi3SetInt64PointerTYPE   fmi3SetInt64Pointer;
FMI3_Export fmi3SetUInt64PointerTYPE  fmi3SetUInt64Pointer;
FMI3_Export fmi3SetBooleanPointerTYPE fmi3SetBooleanPointer;
FMI3_Export fmi3SetStringPointerTYPE  fmi3SetStringPointer;
FMI3_Export fmi3SetBinaryPointerTYPE  fmi3SetBinaryPointer;
FMI3_Export fmi3SetClockPointerTYPE   fmi3SetClockPointer;

#ifdef __cplusplus
}  /* end of extern "C" { */
#endif

#endif /* fmi3DirectMemory_h */
