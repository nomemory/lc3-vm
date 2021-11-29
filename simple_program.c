#include <stdio.h>
#include <stdlib.h>

uint16_t program[] = 
{
    0x3000, //  0011 0000 0000 0000     ;start the program at location x3000
    0x3000, //  0011 0000 0000 0000     ;start the program at location x3000
    0x5260, //  0101 0010 0110 0000     ;clear R1, to be used for the running sum
    0x5920, //  0101 1001 0010 0000     ;clear R4, to be used as a counter
    0x192A, //  0001 1001 0010 1010     ;load R4 with #10, the number of times to add
    0xE4FC, //  1110 0100 1111 1100     ;load the starting address of the data
    0x6680, //  0110 0110 1000 0000     ;load the next number to be added
    0x14A1, //  0001 0100 1010 0001     ;increment the pointer
    0x1243, //  0001 0010 0100 0011     ;add the next number to the running sum
    0x193F, //  0001 1001 0011 1111     ;decrement the counter
    0x03FB, //  0000 0011 1111 1011     ;do it again if the counter is not yet zero
    0xF025  //  1111 0000 0010 0101     ;halt
};

int main(void) {
    char *outf = "out.obj";
    FILE *f = fopen(outf, "wb");
    if (NULL==f) {
        fprintf(stderr, "Cannot write to file %s\n", outf);
    }
    size_t writ = fwrite(program, sizeof(uint16_t), sizeof(program), f);
    fprintf(stdout, "Written size_t=%lu to file %s\n", writ, outf);
    fclose(f);
    return 0;
}