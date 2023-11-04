.code

; int ASMTest(int a, int b, int outData[])
ASMTest     proc public
    vpxord          zmm0, zmm0, zmm0
    mov     eax, 0ffffffffH
    vmovd   xmm1, eax
    vpshufd         xmm1, xmm1, 0
    vinsertf128     ymm1, ymm1, xmm1, 1
    vinserti64x4    zmm1, zmm1, ymm1, 1
    mov     eax, 5a5aH
    kmovd   k1, eax
    vpaddd          zmm1 {k1} {z}, zmm1, zmm0
    vmovdqa64       [r8], zmm1

    sub     ecx, edx
    mov     eax, ecx
    inc     eax
    ret

ASMTest     endp

end

