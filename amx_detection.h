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


#ifndef __AMX_DETECTION_H__
#define __AMX_DETECTION_H__

#define BIT_CHECK(Variable, Bit) ((Variable & (1<<Bit))>>Bit)
#define BIT_FIELD_CHECK(Variable, StartBit, StopBit) ((Variable & (((((1<<(StopBit))-1) ^ ((1<<StartBit)-1)) | (1<<(StopBit)))))>>StartBit)
#define CREATE_BITMASK(StartBit, StopBit)  ((((1<<(StopBit))-1) ^ ((1<<StartBit)-1)) | (1<<(StopBit)))

typedef enum _REGISTER_NAME {
    RegisterEax = 0,
    RegisterEbx,
    RegisterEcx,
    RegisterEdx
} REGISTER_NAME, *PREGISTER_NAME;

typedef struct _CPUID_REGISTERS {

    union {
        unsigned int Registers[4];
        struct {
            unsigned int Eax;
            unsigned int Ebx;
            unsigned int Ecx;
            unsigned int Edx;
        } Register;
    } x;

} CPUID_REGISTERS, *PCPUID_REGISTERS;

/*
 * Prototypes OS Specific Functions
 */
void Os_Platform_Read_Cpuid(unsigned int leaf, unsigned int Subleaf, PCPUID_REGISTERS pCpuidRegisters);
void Os_Platform_Read_xgetbv(unsigned int EcxValue, long long int *pXCR0);

#endif


