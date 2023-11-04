#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>

#ifdef _WIN32
#include <intrin.h>

#define my_cpuid(funcID, eax, ebx, ecx, edx)    do { \
                                                    int regs[4] = { eax, ebx, ecx, edx };   \
                                                    __cpuid(regs, funcID);  \
                                                    eax = regs[0]; ebx = regs[1]; ecx = regs[2]; edx = regs[3]; \
                                                } while(false)

#else
#include <x86intrin.h>
#include <cpuid.h>

#define my_cpuid(funcID, eax, ebx, ecx, edx)    __cpuid(funcID, eax, ebx, ecx, edx)

#endif // _WIN32

extern int ASMTest(int a, int b, int outData[16]);

static void ListAVX512Features(void)
{
    // List basic info
    unsigned regs[16] = { 0 };
    unsigned eax = 0, ebx = 0, ecx = 0, edx = 0;
    my_cpuid(0x00U, eax, ebx, ecx, edx);
    regs[0] = ebx;
    regs[1] = edx;
    regs[2] = ecx;
    printf("Maximum Input Value for Basic CPUID Information: %d, and info: %s\n", eax, (const char*)regs);

    eax = 0; ebx = 0; ecx = 0; edx = 0;
    my_cpuid(0x80000002U, eax, ebx, ecx, edx);
    regs[0] = eax;
    regs[1] = ebx;
    regs[2] = ecx;
    regs[3] = edx;

    eax = 0; ebx = 0; ecx = 0; edx = 0;
    my_cpuid(0x80000003U, eax, ebx, ecx, edx);
    regs[4] = eax;
    regs[5] = ebx;
    regs[6] = ecx;
    regs[7] = edx;

    eax = 0; ebx = 0; ecx = 0; edx = 0;
    my_cpuid(0x80000004U, eax, ebx, ecx, edx);
    regs[8] = eax;
    regs[9] = ebx;
    regs[10] = ecx;
    regs[11] = edx;
    printf("Processor Brand: %s\n", (const char*)regs);

    // List CPU ISA Features
    eax = 0x00U; ebx = 0; ecx = 0; edx = 0;
    my_cpuid(0x07U, eax, ebx, ecx, edx);
    printf("Support SGX (Intel Software Guard Extensions): %s\n", (ebx & (1U << 2)) != 0 ? "YES" : "NO");
    printf("Support BMI1 (Bit Manipulate Instruction 1): %s\n", (ebx & (1U << 3)) != 0 ? "YES" : "NO");
    printf("Support HLE (Hardware Lock Elision): %s\n", (ebx & (1U << 4)) != 0 ? "YES" : "NO");
    printf("Support AVX2 (Advanced Vector Extension 2): %s\n", (ebx & (1U << 5)) != 0 ? "YES" : "NO");
    printf("Support BMI2 (Bit Manipulate Instruction 2): %s\n", (ebx & (1U << 8)) != 0 ? "YES" : "NO");
    printf("Support MPX (Intel Memory Protection Extension): %s\n", (ebx & (1U << 14)) != 0 ? "YES" : "NO");
    printf("Support RDT-A (Intel Resource Director Technology): %s\n", (ebx & (1U << 15)) != 0 ? "YES" : "NO");
    printf("Support AVX512F (AVX-512 Foundation): %s\n", (ebx & (1U << 16)) != 0 ? "YES" : "NO");
    printf("Support AVX512DQ (AVX-512 Doubleword and Quadword Instructions): %s\n", (ebx & (1U << 17)) != 0 ? "YES" : "NO");
    printf("Support AVX512_IFMA (AVX-512 Integer Fused Multiply Add): %s\n", (ebx & (1U << 21)) != 0 ? "YES" : "NO");
    printf("Support AVX512PF (AVX-512 Prefetch Instructions): %s\n", (ebx & (1U << 26)) != 0 ? "YES" : "NO");
    printf("Support AVX512ER (AVX-512 Exponential and Reciprocal Instructions): %s\n", (ebx & (1U << 27)) != 0 ? "YES" : "NO");
    printf("Support AVX512CD (AVX-512 Conflict Detection Instructions): %s\n", (ebx & (1U << 28)) != 0 ? "YES" : "NO");
    printf("Support AVX512BW (AVX-512 Byte and Word Instructions): %s\n", (ebx & (1U << 30)) != 0 ? "YES" : "NO");
    printf("Support AVX512VL (AVX-512 Vector Length Extensions): %s\n", (ebx & (1U << 31)) != 0 ? "YES" : "NO");

    printf("Support AVX512_VBMI (AVX-512 Vector Byte Manipulation Instructions): %s\n", (ecx & (1U << 1)) != 0 ? "YES" : "NO");
    printf("Support AVX512_VBMI2 (AVX-512 Vector Byte Manipulation Instructions 2): %s\n", (ecx & (1U << 6)) != 0 ? "YES" : "NO");
    printf("Support AVX512_VNNI (AVX-512 Vector Neural Network Instructions): %s\n", (ecx & (1U << 11)) != 0 ? "YES" : "NO");
    printf("Support AVX512_BITALG (AVX-512 Bit Algorithms): %s\n", (ecx & (1U << 12)) != 0 ? "YES" : "NO");
    printf("Support AVX512_VPOPCNTDQ (AVX-512 Vector population count instruction): %s\n", (ecx & (1U << 14)) != 0 ? "YES" : "NO");

    printf("Support AVX512_4VNNIW (AVX-512 Vector Neural Network Instructions Word variable precision): %s\n", (edx & (1U << 2)) != 0 ? "YES" : "NO");
    printf("Support AVX512_4FMAPS (AVX-512 Fused Multiply Accumulation Packed Single precision): %s\n", (edx & (1U << 3)) != 0 ? "YES" : "NO");
    printf("Support AVX512_VP2INTERSECT (AVX-512 Vector Pair Intersection to a Pair of Mask Registers): %s\n", (edx & (1U << 8)) != 0 ? "YES" : "NO");

    eax = 0U; ebx = 0U; ecx = 1U; edx = 0U;
    my_cpuid(0x07U, eax, ebx, ecx, edx);
    printf("Support AVX-VNNI.AVX (AVX (VEX-encoded) versions of the Vector Neural Network Instructions): %s\n", (eax & (1U << 4)) != 0 ? "YES" : "NO");
    printf("Support AVX512_BF16 (Vector Neural Network Instructions supporting BFLOAT16 inputs and conversion instructions from IEEE single precision): %s\n", (eax & (1U << 5)) != 0 ? "YES" : "NO");
}

int main(int argc, const char* argv[])
{
    ListAVX512Features();

    puts("\n======== ========\n");

    alignas(64) int buffer[16];
    const int resA = ASMTest(8, 3, buffer);
    printf("resA = %d\n", resA);

    puts("The following sequance: \n");
    for(int i = 0; i < 8; ++i) {
        printf("0x%08x  ", buffer[i]);
    }
    puts("");
    for (int i = 8; i < 16; ++i) {
        printf("0x%08x  ", buffer[i]);
    }
    
    puts("\n==== ====\n");
}

