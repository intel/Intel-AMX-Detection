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


#include <immintrin.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include "amx_detection.h"

/*
 * Note that this usage relies on Intel AMX being enabled via XFD support that would allow XCR0 to set these
 * feature bits but not allow their usage. This both prevent exiting to VMM in order to update XCR0 and provide
 * a method of Opt-In to enable usage for an application.
 * 
 * Linux does require an Opt-In and provides APIs that allow you to both check the feature bits and then perform
 * an Opt-In to use Intel AMX features. This sample code is only looking to detect Intel AMX support and not use 
 * it so it does not perform an Opt-In. 
 *  
 * For more information on Intel AMX support and usage in Linux, See: https://docs.kernel.org/arch/x86/xstate.html
 *  
 */

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
    unsigned int ReturnEax;
    unsigned int ReturnEbx;
    unsigned int ReturnEcx;
    unsigned int ReturnEdx;
    
    asm ("movl %4, %%eax\n"
         "movl %5, %%ecx\n"
         "CPUID\n"
         "movl %%eax, %0\n"
         "movl %%ebx, %1\n"
         "movl %%ecx, %2\n"
         "movl %%edx, %3\n"
          : "=r" (ReturnEax), "=r" (ReturnEbx), "=r" (ReturnEcx), "=r" (ReturnEdx)
          : "r" (Leaf), "r" (Subleaf)
          : "%eax", "%ebx", "%ecx", "%edx"
    );

     pCpuidRegisters->x.Register.Eax  = ReturnEax;
     pCpuidRegisters->x.Register.Ebx  = ReturnEbx;
     pCpuidRegisters->x.Register.Ecx  = ReturnEcx;
     pCpuidRegisters->x.Register.Edx  = ReturnEdx;
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
    unsigned int ReturnEax;
    unsigned int ReturnEdx;
    
    asm ("movl %2, %%ecx\n"
         "XGETBV\n"
         "movl %%eax, %0\n"
         "movl %%edx, %1\n"
          : "=r" (ReturnEax), "=r" (ReturnEdx)
          : "r" (EcxValue)
          : "%eax", "%edx", "%ecx"
    );

     *pXCR0 = (long long int)((long long int)ReturnEdx<<32) | (long long int)ReturnEax;
}

