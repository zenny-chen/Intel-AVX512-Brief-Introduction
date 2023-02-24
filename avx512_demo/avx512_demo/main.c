#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>

extern int ASMTest(int a, int b, int outData[16]);

int main(void)
{
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
