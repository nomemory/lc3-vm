#include "vm_dbg.h"

// DEBUG
void fprintf_binary(FILE *f, uint16_t num) {
    int c = 16;
    while(c-->0) {
        if ((c+1)%4==0) {
            fprintf(f, " ");
        }
        fprintf(f, "%d", (num>>c)&1);
    }
}

void fprintf_inst(FILE *f, uint16_t instr) {
    fprintf(f, "instr=%u, binary=", instr);
    fprintf_binary(f, instr);
    fprintf(f, "\n");
}

void fprintf_mem(FILE *f, uint16_t *mem, uint16_t from, uint16_t to) {
    for(int i = from; i < to; i++) {
        fprintf(f, "mem[%d]=", i);
        fprintf_binary(f, mem[i]);
        fprintf(f, "\n");
    }
}

void fprintf_mem_nonzero(FILE *f, uint16_t *mem, uint32_t stop) {
    for(int i = 0; i < stop; i++) {
        if (mem[i]!=0) {
           fprintf(f, "mem[%d]=", i);
            fprintf_binary(f, mem[i]);
            fprintf(f, "\n"); 
        }
    }
}

void fprintf_reg(FILE *f, uint16_t *reg, int idx) {
    fprintf(stdout, "reg[%d]=%hu\n", idx, reg[idx]);
}