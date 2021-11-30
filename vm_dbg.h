#include <stdio.h>
#include <stdlib.h>

void fprintf_binary(FILE *f, uint16_t num);
void fprintf_inst(FILE *f, uint16_t instr);
void fprintf_mem(FILE *f, uint16_t *mem, uint16_t from, uint16_t to);
void fprintf_mem_nonzero(FILE *f, uint16_t *mem, uint32_t stop);
void fprintf_reg(FILE *f, uint16_t *reg, int idx);
void fprintf_reg_all(FILE *f, uint16_t *reg, int size);