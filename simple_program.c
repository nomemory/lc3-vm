#include <stdio.h>
#include <stdlib.h>

/*
Our goal is to take the ten numbers which are stored in memory locations x3100 through
x3109, and add them together, leaving the result in register 1.
*/

uint16_t program[] = 
{
    /*mem[0x3000]=*/   0x3000,    //  0011 0000 0000 0000             .ORIG x3000     ;start the program at location x3000                    
    /*mem[0x3001]=*/   0x5260,    //  0101 0010 0110 0000             AND R1,R1,x0    ;clear R1, to be used for the running sum               
    /*mem[0x3002]=*/   0x5920,    //  0101 1001 0010 0000             AND R4,R4,x0    ;clear R4, to be used as a counter                      
    /*mem[0x3003]=*/   0x192A,    //  0001 1001 0010 1010             ADD R4,R4,xA    ;load R4 with #10, the number of times to add           
    /*mem[0x3004]=*/   0xE4FC,    //  1110 0100 1111 1100             LEA R2,x0FC     ;load the starting address of the data          
    /*mem[0x3005]=*/   0x6680,    //  0110 0110 1000 0000     LOOP    LDR R3,R2,x0    ;load the next number to be added                       
    /*mem[0x3006]=*/   0x14A1,    //  0001 0100 1010 0001             ADD R2,R2,x1    ;increment the pointer
    /*mem[0x3007]=*/   0x1243,    //  0001 0010 0100 0011             ADD R1,R1,R3    ;add the next number to the running sum
    /*mem[0x3008]=*/   0x193F,    //  0001 1001 0011 1111             ADD R4,R4,x-1   ;decrement the counter
    /*mem[0x3008]=*/   0x03FB,    //  0000 0011 1111 1011             BRp LOOP        ;do it again if the counter is not yet zero
    /*mem[0x300A]=*/   0xF025     //  1111 0000 0010 0101             HALT            ;halt
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