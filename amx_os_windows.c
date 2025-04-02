/*
 * Copyright (C) 2025 by Intel Corporation
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "amx_detection.h"


void __cpuidex(unsigned int *cpuinfo,int function_id,int subfunction_id);
#pragma intrinsic(__cpuidex)

unsigned __int64 __xgetbv (unsigned int a);
#pragma intrinsic(__xgetbv)



/*
 * Os_Platform_Read_Cpuid
 *
 *    OS/Compiler Specific intrinsic of reading CPUID.
 *
 * Arguments:
 *     Leaf, Subleaf, CPUID Data Structure
 *     
 * Return:
 *     None
 */
void Os_Platform_Read_Cpuid(unsigned int Leaf, unsigned int Subleaf, PCPUID_REGISTERS pCpuidRegisters)
{
    __cpuidex(&pCpuidRegisters->x.Registers[0], Leaf, Subleaf);
}


/*
 * Os_Platform_Read_xgetbv
 *
 *    OS/Compiler Specific intrinsic of reading XGETBV.
 *
 * Arguments:
 *     ECX Input Value, XCR0 Output Value
 *     
 * Return:
 *     None
 */
void Os_Platform_Read_xgetbv(unsigned int EcxValue, long long int *pXCR0)
{
    *pXCR0 = __xgetbv(EcxValue);
}


/*
 * Os_Platform_Permissions_Request_Read_xgetbv
 *
 *    OS/Compiler Specific intrinsic of reading XGETBV.
 *
 * Arguments:
 *     ECX Input Value, XCR0 Output Value
 *     
 * Return:
 *     None
 */
void Os_Platform_Permissions_Request_Read_xgetbv(unsigned int EcxValue, long long int *pXCR0, long long int StateRequest)
{
    StateRequest;
    *pXCR0 = __xgetbv(EcxValue);
}


