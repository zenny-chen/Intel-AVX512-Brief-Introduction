.text
.align 4
.intel_syntax noprefix
.globl ASMTest

// int ASMTest(int a, int b, int outData[])
ASMTest:
    vpxord          zmm0, zmm0, zmm0
    mov     eax, 0xffffffff
    vmovd   xmm1, eax
    vpshufd         xmm1, xmm1, 0
    vinsertf128     ymm1, ymm1, xmm1, 1
    vinserti64x4    zmm1, zmm1, ymm1, 1
    mov     eax, 0x5a5a
    kmovd   k1, eax
    vpaddd          zmm1 {k1} {z}, zmm1, zmm0
    vmovdqa64       [rdx], zmm1

    sub     edi, esi
    mov     eax, edi
    inc     eax
    ret

