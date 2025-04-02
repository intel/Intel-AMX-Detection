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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "amx_detection.h"

/*
 * AMX CPUID Features
 */
typedef struct _CPUID_FEATURE {

    unsigned int Leaf;
    unsigned int Subleaf;
    REGISTER_NAME Register;
    unsigned int Startbit;
    unsigned int Stopbit;
    char *pszFeatureName;
    char *pszFeatureDecription;

} CPUID_FEATURE, *PCPUID_FEATURE;

#define INVALID_LEAF 0xFFFFFFFF

#define XSAVE_TILE_CONFIG_STATE  ((long long int)1<<17)
#define XSAVE_TILE_DATA_STATE    ((long long int)1<<18)
#define XSAVE_AMX_STATE ((long long int)XSAVE_TILE_CONFIG_STATE | XSAVE_TILE_DATA_STATE)

/*
 * These need to be ordered such that the first entry is Subleaf 0 and Subleaf 0 is defined that the maximum subleaf is in EAX.
 */
CPUID_FEATURE g_AmxFeatureList[] = {
    /*
     * CPUID 07H - Structured Extended Feature Flags
     */
    { 7, 0, RegisterEdx, 22, 22, "AMX-BF16", "If 1, the processor supports tile computational operations on bfloat16 numbers." },
    { 7, 0, RegisterEdx, 24, 24, "AMX-TILE", "If 1, the processor supports tile architecture." },
    { 7, 0, RegisterEdx, 25, 25, "AMX-INT8", "If 1, the processor supports tile computational operations on 8-bit integers." },
    { 7, 1, RegisterEax, 21, 21, "AMX-FP16", "If 1, the processor supports tile computational operations on FP16 numbers." },
    { 7, 1, RegisterEdx, 8, 8, "AMX-COMPLEX", "If 1, supports the AMX-COMPLEX instructions." },

    /*
     * CPUID 1DH - Tile Information
     */
    { 0x1D, 0, RegisterEax, 0, 31, "max_palette", "Highest numbered palette Sub-leaf. Value = 1." },
    { 0x1D, 1, RegisterEax, 0, 15, "Total Tile Bytes", "Palette 1 total_tile_bytes. Value = 8192." },
    { 0x1D, 1, RegisterEax, 16, 31, "Bytes Per Tile", "Palette 1 bytes_per_tile. Value = 1024." },
    { 0x1D, 1, RegisterEbx, 0, 15, "Bytes Per Row", "Palette 1 bytes_per_row. Value = 64." },
    { 0x1D, 1, RegisterEbx, 16, 31, "Max Names", "Palette 1 max_names (number of tile registers). Value = 8." },
    { 0x1D, 1, RegisterEcx, 0, 15, "Max Rows", "Palette 1 max_rows. Value = 16." },

    /*
     * CPUID 1EH - TMUL Information
     */
    { 0x1E, 0, RegisterEbx, 0, 7, "tmul_maxk", "tmul_maxk (rows or columns). Value = 16." },
    { 0x1E, 0, RegisterEbx, 8, 23, "tmul_maxn", "tmul_maxn (column bytes). Value = 64." },
    { 0x1E, 1, RegisterEax, 0, 0, "AMX-INT8", "If 1, the processor supports tile computational operations on 8-bit integers." },
    { 0x1E, 1, RegisterEax, 1, 1, "AMX-BF16", "If 1, the processor supports tile computational operations on bfloat16 numbers." },
    { 0x1E, 1, RegisterEax, 2, 2, "AMX-COMPLEX", "If 1, supports the AMX-COMPLEX instructions." },
    { 0x1E, 1, RegisterEax, 3, 3, "AMX-FP16", "If 1, the processor supports tile computational operations on FP16 numbers." },
    { 0x1E, 1, RegisterEax, 4, 4, "AMX-FP8", "If 1, supports Intel AMX computations for the FP8 data type." },
    { 0x1E, 1, RegisterEax, 5, 5, "AMX-TRANSPOSE", "If 1, supports the AMX-TRANSPOSE. instructions." },
    { 0x1E, 1, RegisterEax, 6, 6, "AMX-TF32 (FP19)", "If 1, supports the AMX-TF32 (FP19) instructions." },
    { 0x1E, 1, RegisterEax, 7, 7, "AMX-AVX512", "If 1, supports the AMX-AVX512 instructions." },
    { 0x1E, 1, RegisterEax, 8, 8, "AMX-MOVRS", "If 1, supports the AMX-MOVRS instructions." },

    /*
     * Terminal Entry
     */
    { INVALID_LEAF, 0, 0, 0, 0, NULL, NULL },
};
 

/*
 * Internal Prototypes
 */
void AmxDetection_DisplayAmxFeatures(void);
void AmxDetection_DetectOsSupport(void);

/*
 * main
 *
 * Entry Point
 *
 * Arguments:
 *     argc - Number of Arguements
 *     argv - Arguements 
 *     
 * Return:
 *     Zero
 */
int main(int argc, char **argv)
{
    CPUID_REGISTERS CpuidRegisters;
    unsigned int MAX_LEAF;

    /* 
     *  Leaf 00H - Maximum Input for Basic CPUID and Vendor ID
     *  
     *  CPUID.00H:EAX[31:0] - MAX_LEAF -  Maximum input value for basic CPUID Information.
     */
    Os_Platform_Read_Cpuid(0, 0, &CpuidRegisters);
    MAX_LEAF = CpuidRegisters.x.Register.Eax;

    if (MAX_LEAF >= 7) 
    {
        /* 
         *  Leaf 07H - Structured Extended Feature Flags
         *  
         *  CPUID.07H.00H:EDX[24] - AMX-TILE - If 1, the processor supports tile architecture.
         */
        Os_Platform_Read_Cpuid(7, 0, &CpuidRegisters);
        if (BIT_CHECK(CpuidRegisters.x.Register.Edx, 24) == 1) 
        {
            /* 
             *  Leaf 1DH - Tile Information
             *  Leaf 1EH - TMUL Information
             *  
             */
            if (MAX_LEAF >= 0x1E)
            {
                AmxDetection_DisplayAmxFeatures();
                AmxDetection_DetectOsSupport();
            }
            else
            {
                printf("Invalid CPUID Reporting, missing AMX CPUID leaves\n");
            }
        }
        else
        {
            printf("AMX Not Supported\n");
        }
    }
    else
    {
        printf("AMX Not Supported\n");
    }

    return 0;
}




/*
 * AmxDetection_DetectOsSupport
 *
 * Detect OS Support of AMX
 *
 * Arguments:
 *     None
 *     
 * Return:
 *     None
 */
void AmxDetection_DetectOsSupport(void)
{
   CPUID_REGISTERS CpuidRegisters;
   long long int XCR0Value;

   /*
    *  CPUID 01H - Version Information: Type, Family, Model and Stepping ID
    *  
    *    The platform must support XSAVE and the OS must have XSAVE enabled.
    *    This will be true on modern OS.
    *  
    *  CPUID.01H:ECX[26] - XSAVE - A value of 1 indicates that the processor supports the XSAVE/XRSTOR processor extended states feature, the XSETBV/XGETBV instructions, and XCR0.
    *  CPUID.01H:ECX[27] - OSXSAVE - A value of 1 indicates that the OS has set CR4.OSXSAVE[bit 18] to enable XSETBV/XGETBV instructions to access XCR0 and to support processor extended state management using XSAVE/XRSTOR.
    *  
    *  This tests if the current logical processor has CR4.OSXSAVE[18] set; however the OS will symmetrically set this bit across logical processors.
    *  
    */
   Os_Platform_Read_Cpuid(1, 0, &CpuidRegisters);

   if (BIT_CHECK(CpuidRegisters.x.Register.Ecx, 26) == 1) 
   {
       if (BIT_CHECK(CpuidRegisters.x.Register.Ecx, 27) == 1) 
       {
           Os_Platform_Read_xgetbv(0, &XCR0Value);

           if ((XCR0Value & (XSAVE_AMX_STATE)) == (XSAVE_AMX_STATE)) 
           {
               printf("AMX State is supported by the OS\n");
           }
           else
           {
               printf("AMX State is not supported by the OS\n");
           }
       }
       else
       {
           printf("The OS does not support XSAVE\n");
       }
   }
   else
   {
       printf("Platform does not support XSAVE\n");
   }
}





/*
 * AmxDetection_DisplayAmxFeatures
 *
 * Display features of AMX
 *
 * Arguments:
 *     None
 *     
 * Return:
 *     None
 */
void AmxDetection_DisplayAmxFeatures(void)
{
    CPUID_REGISTERS CpuidRegisters;
    unsigned int Index;
    unsigned int CurrentLeaf;
    unsigned int CurrentSubleaf;
    unsigned int MaximumSubleaf;
    unsigned int DisplayIsValid;
    unsigned int Value;
    char *pszRegisterName[] = { "EAX", "EBX", "ECX", "EDX" };

    /*
     * The checks required before calling this function is that AMX is supported by the processor and the maximum leaf >= 0x1E. 
     *  
     * Adding any leaves beyond 0x1E would require more checks. 
     *  
     */
    Index = 0;
    Os_Platform_Read_Cpuid(g_AmxFeatureList[Index].Leaf, g_AmxFeatureList[Index].Subleaf, &CpuidRegisters);

    MaximumSubleaf = CpuidRegisters.x.Register.Eax;
    CurrentLeaf    = g_AmxFeatureList[Index].Leaf;
    CurrentSubleaf = g_AmxFeatureList[Index].Subleaf;


    do {

        DisplayIsValid = 1;

        /*
         * Cache the next CPUID Leaf, Subleaf
         */
        if (CurrentLeaf != g_AmxFeatureList[Index].Leaf)
        {
            /*
             *  Structure is invalid if Subleaf is not 0 and CPUID.Leaf.Subleaf:EAX is not maximum leaf.
             */
            Os_Platform_Read_Cpuid(g_AmxFeatureList[Index].Leaf, g_AmxFeatureList[Index].Subleaf, &CpuidRegisters);

            /*
             * The strucutre is only supporting leaves where CPUID.Leaf.00H:EAX[31:0] marks the maximum supported subleaf. 
             *  
             * Cache the current Leaf, Subleaf values. 
             */
            CurrentLeaf    = g_AmxFeatureList[Index].Leaf;
            CurrentSubleaf = g_AmxFeatureList[Index].Subleaf;
            MaximumSubleaf = CpuidRegisters.x.Register.Eax;
        }
        else
        {
            /*
             * Check if we have a new subleaf of the current leaf and cache.
             */
            if (CurrentSubleaf != g_AmxFeatureList[Index].Subleaf)
            {

                /*
                 * The strucutre is only supporting leaves where CPUID.Leaf.00H:EAX[31:0] marks the maximum supported subleaf.
                 */
                DisplayIsValid = 0;
                if (MaximumSubleaf >= g_AmxFeatureList[Index].Subleaf) 
                {
                    Os_Platform_Read_Cpuid(CurrentLeaf, g_AmxFeatureList[Index].Subleaf, &CpuidRegisters);
                    CurrentSubleaf = g_AmxFeatureList[Index].Subleaf;
                    DisplayIsValid = 1;
                }
            }
        }

        /*
         * The Display would only be invalid if we have moved to an unsupported subleaf, in that case we skip those subleaves.
         *  
         */
        if (DisplayIsValid) 
        {
            if (g_AmxFeatureList[Index].Stopbit == g_AmxFeatureList[Index].Startbit) 
            {
                Value = BIT_CHECK(CpuidRegisters.x.Registers[g_AmxFeatureList[Index].Register], g_AmxFeatureList[Index].Startbit);

                printf("\nCPUID.%02xH.%02xH:%s[%i] = %i\n", CurrentLeaf, CurrentSubleaf, pszRegisterName[g_AmxFeatureList[Index].Register], g_AmxFeatureList[Index].Startbit, Value);
            }
            else
            {
                if (g_AmxFeatureList[Index].Stopbit == 31 && g_AmxFeatureList[Index].Startbit == 0) 
                {
                    Value = CpuidRegisters.x.Registers[g_AmxFeatureList[Index].Register];
                }
                else
                {
                    Value = BIT_FIELD_CHECK(CpuidRegisters.x.Registers[g_AmxFeatureList[Index].Register], g_AmxFeatureList[Index].Startbit, g_AmxFeatureList[Index].Stopbit);
                }

                printf("\nCPUID.%02xH.%02xH:%s[%i:%i] = %i\n", CurrentLeaf, CurrentSubleaf, pszRegisterName[g_AmxFeatureList[Index].Register], g_AmxFeatureList[Index].Stopbit, g_AmxFeatureList[Index].Startbit, Value);
            }
            printf("   %s - %s\n", g_AmxFeatureList[Index].pszFeatureName, g_AmxFeatureList[Index].pszFeatureDecription);
        }

        Index++;
    } while (g_AmxFeatureList[Index].Leaf != INVALID_LEAF);
}


