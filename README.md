# Intel AMX Detection

This is an application that detects the support of Intel AMX in the processor and enablement by the operating system.  This sample does not show how to use Intel AMX instructions or enable applications to use Intel AMX.

## License
BSD Zero Clause License

Copyright (C) 2025 Intel Corporation

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. 
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


## Source List
### OS-Agnostic Files
 - **amx_detection.h** - This is the common header file included across all soruce files for sharing structures and shared functions.
 - **amx_detection.c** - The OS Agnostic entry point for this example which will determine Intel AMX support.

### OS-Dependent Files

The following files are the OS-Dependent APIs which only need to be included for the specific OS.

 - **amx_os_linux.c** - The OS-Dependent APIs for Linux.
 - **amx_os_windows.c** - The OS-Dependent APIs for Windows.

## How to Compile

### Linux

You may use any compiler for linux that you wish to use.  The following example shows how to compile the sources for GCC using the command line.

```
    gcc -g -c -Wall  amx_os_linux.c
    gcc -g  amx_detection.c -Wall -o ./amxdetection.o amx_os_linux.o 
```

### Windows

You are free to use any compiler you need to use for Windows OS.  The following example shows how to compile the sources using the Microsoft compiler from the command line.
```
    cl /c amx_detection.c amx_os_windows.c /IF:\WDK\Win7\inc\crt /Ox
    link amx_os_windows.obj amx_detection.obj /nodefaultlib F:\WDK\Win7\lib\win7\amd64\kernel32.lib F:\WDK\Win7\lib\crt\amd64\msvcrt.lib  /out:amxdetection.exe
```

### Other Operating Systems

This project can easily be moved to another OS that supports a standard C runtime compiler.  The following APIs would need to be implemented for OS-Specific implemenation and compiled into the sources.

 - **void Os_Platform_Read_Cpuid(unsigned int leaf, unsigned int Subleaf, PCPUID_REGISTERS pCpuidRegisters)**

 -- This implementation requests to execute the CPUID x86 instruction, it is in the OS-Specific section more for being a compiler-specific assembly/intrinsic implementation.

 - **void Os_Platform_Read_xgetbv(unsigned int EcxValue, long long int *pXCR0)**

 -- This implementation requests to execute the XGETBV x86 instruction, it is in the OS-Specific section more for being a compiler-specific assembly/intrinsic implementation.

## How to use the application

      






