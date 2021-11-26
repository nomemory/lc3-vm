#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define OPC(i) ((i)<<12)
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

enum regist { R0 = 0, R1, R2, R3, R4, R5, R6, R7, RPC, RCND, RCNT };
enum flags { FP = 1 << 0, FZ = 1 << 1, FN = 1 << 2 };
enum m_regist { MR_KBSR = 0xFE00, MR_KBDR = 0xFE02 };

uint16_t mem[UINT16_MAX];
uint16_t reg[RCNT];

static inline uint16_t mr(uint16_t address) { return mem[address]; }
static inline void mw(uint16_t address, uint16_t val) { mem[address] = val; }
static inline uint16_t sext(uint16_t n, int b) { return ((n>>(b-1))&1) ? (n|(0xFFFF << b)) : n; }
static inline void uf(enum regist r) {
    if (reg[r]==0) reg[RCND] = FZ;
    else if (reg[r]>>15) reg[RCND] = FN;
    else reg[RCND] = FP;
}
static inline void add(uint16_t i) { reg[DR(i)] = reg[SR(i)] + FIMM(i) ? SEXTIMM(i) : reg[SR2(i)]; uf(DR(i)); }
static inline void and(uint16_t i) { reg[DR(i)] = SR1(i) & (FIMM(i) ? SEXTIMM(i) : SR2(i)); uf(DR(i)); }
static inline void ldi(uint16_t i) { reg[DR(i)] = mr(mr(reg[RPC]+POFF9(i))); uf(DR(i)); }
static inline void not(uint16_t i) { reg[DR(i)]=~(SR1(i)); uf(DR(i)); }
static inline void br(uint16_t i) { if (reg[RCND] & FCND(i)) { reg[RPC] += POFF9(i); } }
static inline void jsr(uint16_t i) { reg[R7] = reg[RPC]; reg[RPC] = (FL(i)) ? reg[RPC] + POFF11(i) : BR(i); }
static inline void jmp(uint16_t i) { reg[RPC] = reg[BR(i)]; }
static inline void ld(uint16_t i) { reg[DR(i)] = mr(reg[RPC] + POFF9(i)); uf(DR(i)); }
static inline void ldr(uint16_t i) {reg[DR(i)] = mr(SR1(i) + POFF(i)); uf(reg[DR(i)]); }
static inline void lea(uint16_t i) { reg[DR(i)] =reg[RPC] + POFF9(i); uf(reg[DR(i)]); }
static inline void st(uint16_t i) { mw(reg[RPC] + POFF9(i), DR(i)); }
static inline void sti(uint16_t i) { mw(mr(reg[RPC] + POFF9(i)), DR(i)); }
static inline void str(uint16_t i) { mw(reg[SR1(i)] + POFF(i), DR(i)); }
static inline void rti(uint16_t i) {} // unused
static inline void res(uint16_t i) {} // unused
static inline void trap(uint16_t i) {}

typedef void (*op_ex_f)(uint16_t i);
op_ex_f op_ex[] = { br, add, ld, st, jsr, and, ldr, str, rti, not, ldi, sti, jmp, res, lea, /*op_trap*/ };

int main(void) {
    enum { PC_START = 0x300 };
    reg[RPC] = PC_START;
    bool running = true;
    while(running) {
        uint16_t i = mr(reg[RPC]++);
        op_ex[OPC(i)](i);
    }
    return 0;
}