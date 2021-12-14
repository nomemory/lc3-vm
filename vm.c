#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "vm_dbg.h"

#define NOPS (16)

#define OPC(i) ((i)>>12)
#define DR(i) (((i)>>9)&0x7)
#define SR1(i) (((i)>>6)&0x7)
#define SR2(i) ((i)&0x7)
#define FIMM(i) ((i>>5)&01)
#define IMM(i) ((i)&0x1F)
#define SEXTIMM(i) sext(IMM(i),5)
#define FCND(i) (((i)>>9)&0x7)
#define POFF(i) sext((i)&0x3F, 6)
#define POFF9(i) sext((i)&0x1FF, 9)
#define POFF11(i) sext((i)&0x7FF, 11)
#define FL(i) (((i)>>11)&1)
#define BR(i) (((i)>>6)&0x7)
#define TRP(i) ((i)&0xFF)

bool running = true;

typedef void (*op_ex_f)(uint16_t i);
typedef void (*trp_ex_f)();

enum { trp_offset = 0x20 };
enum regist { R0 = 0, R1, R2, R3, R4, R5, R6, R7, RPC, RCND, RCNT };
enum flags { FP = 1 << 0, FZ = 1 << 1, FN = 1 << 2 };

uint16_t mem[UINT16_MAX] = {0};
uint16_t reg[RCNT] = {0};
uint16_t PC_START = 0x3000;

static inline uint16_t mr(uint16_t address) { return mem[address];  }
static inline void mw(uint16_t address, uint16_t val) { mem[address] = val; }
static inline uint16_t sext(uint16_t n, int b) { return ((n>>(b-1))&1) ? (n|(0xFFFF << b)) : n; }
static inline void uf(enum regist r) {
    if (reg[r]==0) reg[RCND] = FZ;
    else if (reg[r]>>15) reg[RCND] = FN;
    else reg[RCND] = FP;
}
static inline void add(uint16_t i)  { reg[DR(i)] = reg[SR1(i)] + (FIMM(i) ? SEXTIMM(i) : reg[SR2(i)]); uf(DR(i)); }
static inline void and(uint16_t i)  { reg[DR(i)] = reg[SR1(i)] & (FIMM(i) ? SEXTIMM(i) : reg[SR2(i)]); uf(DR(i)); }
static inline void ldi(uint16_t i)  { reg[DR(i)] = mr(mr(reg[RPC]+POFF9(i))); uf(DR(i)); }
static inline void not(uint16_t i)  { reg[DR(i)]=~(SR1(i)); uf(DR(i)); }
static inline void br(uint16_t i)   { if (reg[RCND] & FCND(i)) { reg[RPC] += POFF9(i); } }
static inline void jsr(uint16_t i)  { reg[R7] = reg[RPC]; reg[RPC] = (FL(i)) ? reg[RPC] + POFF11(i) : BR(i); }
static inline void jmp(uint16_t i)  { reg[RPC] = reg[BR(i)]; }
static inline void ld(uint16_t i)   { reg[DR(i)] = mr(reg[RPC] + POFF9(i)); uf(DR(i)); }
static inline void ldr(uint16_t i)  { reg[DR(i)] = mr(reg[SR1(i)] + POFF(i)); uf(DR(i)); }
static inline void lea(uint16_t i)  { reg[DR(i)] =reg[RPC] + POFF9(i); uf(DR(i)); }
static inline void st(uint16_t i)   { mw(reg[RPC] + POFF9(i), reg[DR(i))]; }
static inline void sti(uint16_t i)  { mw(mr(reg[RPC] + POFF9(i)), reg[DR(i)]); }
static inline void str(uint16_t i)  { mw(reg[SR1(i)] + POFF(i), reg[DR(i)]); }
static inline void rti(uint16_t i) {} // unused
static inline void res(uint16_t i) {} // unused
static inline void tgetc() { reg[R0] = getchar(); }
static inline void tout() { fprintf(stdout, "%c", (char)reg[R0]); }
static inline void tputs() {
    uint16_t *p = mem + reg[R0];
    while(*p) {
        fprintf(stdout, "%c", (char)*p);
        p++;
    }
}
static inline void tin() { reg[R0] = getchar(); fprintf(stdout, "%c", reg[R0]); }
static inline void tputsp() { /* Not Implemented */ }
static inline void thalt() { running = false; } 
static inline void tinu16() { fscanf(stdin, "%hu", &reg[R0]); }
static inline void toutu16() { fprintf(stdout, "%hu\n", reg[R0]); }
trp_ex_f trp_ex[8] = { tgetc, tout, tputs, tin, tputsp, thalt, tinu16, toutu16 };
static inline void trap(uint16_t i) { trp_ex[TRP(i)-trp_offset](); }
op_ex_f op_ex[NOPS] = { /*0*/ br, add, ld, st, jsr, and, ldr, str, rti, not, ldi, sti, jmp, res, lea, trap };
void start(uint16_t offset) { 
    reg[RPC] = PC_START + offset;
    while(running) {
        uint16_t i = mr(reg[RPC]++);
        op_ex[OPC(i)](i);
    }
}
void ld_img(char *fname, uint16_t offset) {
    FILE *in = fopen(fname, "rb");
    if (NULL==in) {
        fprintf(stderr, "Cannot open file %s.\n", fname);
        exit(1);    
    }
    uint16_t *p = mem + PC_START + offset;
    fread(p, sizeof(uint16_t), (UINT16_MAX-PC_START), in);
    fclose(in);
}
int main(int argc, char **argv) {
    ld_img(argv[1], 0x0);
    fprintf(stdout, "Occupied memory after program load:\n");
    fprintf_mem_nonzero(stdout, mem, UINT16_MAX);
    start(0x0); // START PROGRAM
    fprintf(stdout, "Occupied memory after program execution:\n");
    fprintf_mem_nonzero(stdout, mem, UINT16_MAX);
    fprintf(stdout, "Registers after program execution:\n");
    fprintf_reg_all(stdout, reg, RCNT);
    return 0;
}
