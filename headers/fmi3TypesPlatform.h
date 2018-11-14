#ifndef fmi3TypesPlatform_h
#define fmi3TypesPlatform_h

/*
Standard header file to define the argument types of the
functions of the Functional Mock-up Interface 3.0-wg003.3.
This header file must be utilized both by the model and
by the simulation engine.

Copyright (C) 2008-2011 MODELISAR consortium,
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

/* Platform (unique identification of this header file) */
#define fmi3TypesPlatform "default"

/*
Type definitions of variables passed as arguments
Version "default" means:

fmi3Component           : an opaque object pointer
fmi3ComponentEnvironment: an opaque object pointer
fmi3FMUstate            : an opaque object pointer
fmi3ValueReference      : value handle type
fmi3Real                : double precision floating-point data type
fmi3Integer             : basic signed integer data type
fmi3Boolean             : datatype to be used with fmi3True and fmi3False
fmi3Char                : character data type (size of one character)
fmi3String              : a pointer to a vector of fmi3Char characters
                          ('\0' terminated, UTF8 encoded)
fmi3Byte                : smallest addressable unit of the machine (typically one byte)
fmi3Binary              : a pointer to a vector of fmi3Byte bytes (out-of-band length terminated)
*/

/* tag::Component[] */
typedef void*           fmi3Component;             /* Pointer to FMU instance */
/* end::Component[] */

/* tag::ComponentEnvironment[] */
typedef void*           fmi3ComponentEnvironment;  /* Pointer to FMU environment */
/* end::ComponentEnvironment[] */

/* tag::FMUstate[] */
typedef void*           fmi3FMUstate;              /* Pointer to internal FMU state */
/* end::FMUstate[] */

/* tag::ValueReference[] */
typedef unsigned int    fmi3ValueReference;        /* Handle to the value of a variable */
/* end::ValueReference[] */

/* tag::VariableTypes[] */
typedef double          fmi3Real;     /* Data type for floating point real numbers */
typedef int             fmi3Integer;  /* Data type for signed integer numbers */
typedef int             fmi3Boolean;  /* Data type for Boolean numbers
                                         (only two values: fmi3False, fmi3True) */
typedef char            fmi3Char;     /* Data type for one character */
typedef const fmi3Char* fmi3String;   /* Data type for character strings 
					 ('\0' terminated, UTF8 encoded) */
typedef char            fmi3Byte;     /* Smallest addressable unit of the machine
					 (typically one byte) */
typedef const fmi3Byte* fmi3Binary;   /* Data type for binary data
                                         (out-of-band length terminated) */

/* Values for fmi3Boolean  */
#define fmi3True  1
#define fmi3False 0
/* end::VariableTypes[] */

#endif /* fmi3TypesPlatform_h */
