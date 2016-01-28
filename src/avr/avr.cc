#include "avr.h"

#include <iostream>
#include <cstdio>

namespace coresim {

std::string avr::name = "AVR";

avr::avr(vmem &m, vio &i) : core(m, i) {
	reset();
}

void avr::reset() {
	pc = 0;
	sp = 0xffff;
	std::memset(&regs, 0, sizeof(regs));
	std::memset(&sreg, 0, sizeof(sreg));
	cycles = 0;
}

void avr::debug() {
	core::debug();

	std::printf("-> Stopped at pc=0x%04x\n", pc << 1);

	if (is_verbose) {
		for (int i = 0; i < 32; i+=4) {
			std::printf("r%02d=0x%02x r%02d=0x%02x r%02d=0x%02x r%02d=0x%02x\n",
			            i, regs[i], i+1, regs[i+1], i+2, regs[i+2], i+3,regs[i+3]);
		}
	}
}

// instruction execution

namespace {

enum {
MASK_OP16  = 0xffff,
	NOP    = 0x0000,
	IJMP   = 0x9409,
	EIJMP  = 0x9419,
	SEC    = 0x9408,
	SEZ    = 0x9418,
	SEN    = 0x9428,
	SEV    = 0x9438,
	SES    = 0x9448,
	SEH    = 0x9458,
	SET    = 0x9468,
	SEI    = 0x9478,
	CLC    = 0x9488,
	CLZ    = 0x9498,
	CLN    = 0x94a8,
	CLV    = 0x94b8,
	CLS    = 0x94c8,
	CLH    = 0x94d8,
	CLT    = 0x94e8,
	CLI    = 0x94f8,
	ICALL  = 0x9509,
	EICALL = 0x9519,
	RET    = 0x9508,
	RETI   = 0x9518,
	SLEEP  = 0x9588,
	BREAK  = 0x9598,
	WDR    = 0x95a8,
	LPM0   = 0x95c8,
	ELPM0  = 0x95d8,
	SPM    = 0x95e8,
MASK_OP11  = 0xfe0f,
	POP    = 0x900f,
	PUSH   = 0x920f,
	LDS32  = 0x9000,
	STS32  = 0x9200,
	COM    = 0x9400,
	NEG    = 0x9401,
	SWAP   = 0x9402,
	INC    = 0x9403,
	ASR    = 0x9404,
	LSR    = 0x9405,
	ROR    = 0x9406,
	DEC    = 0x9407,
	LDX    = 0x900c,
	LDXI   = 0x900d,
	LDXD   = 0x900e,
	LDY    = 0x8008,
	LDYI   = 0x9009,
	LDYD   = 0x900a,
	LDZ    = 0x8000,
	LDZI   = 0x9001,
	LDZD   = 0x9002,
	STX    = 0x920c,
	STXI   = 0x920d,
	STXD   = 0x920e,
	STY    = 0x8208,
	STYI   = 0x9209,
	STYD   = 0x920a,
	STZ    = 0x8200,
	STZI   = 0x9201,
	STZD   = 0x9202,
MASK_OP10M = 0xff88,
	MULSU  = 0x0300,
	FMUL   = 0x0308,
	FMULS  = 0x0380,
	FMULSU = 0x0388,
MASK_OP10J = 0xfe0e,
	LPM    = 0x9004,
	ELPM   = 0x9006,
	JMP    = 0x940c,
	CALL   = 0x940e,
MASK_OP9   = 0xfc07,
	BRCS   = 0xf000,
	BREQ   = 0xf001,
	BRMI   = 0xf002,
	BRVS   = 0xf003,
	BRLT   = 0xf004,
	BRHS   = 0xf005,
	BRTS   = 0xf006,
	BRIE   = 0xf007,
	BRCC   = 0xf400,
	BRNE   = 0xf401,
	BRPL   = 0xf402,
	BRVC   = 0xf403,
	BRGE   = 0xf404,
	BRHC   = 0xf405,
	BRTC   = 0xf406,
	BRID   = 0xf407,
MASK_OP8L  = 0xff00,
	MOVW   = 0x0100,
	MULS   = 0x0200,
	ADIW   = 0x9600,
	SBIW   = 0x9700,
	CBI    = 0x9800,
	SBIC   = 0x9900,
	SBI    = 0x9a00,
	SBIS   = 0x9b00,
MASK_OP8B  = 0xfe08,
	BLD    = 0xf800,
	BST    = 0xfa00,
	SBRC   = 0xfc00,
	SBRS   = 0xfe00,
MASK_OP6   = 0xfc00,
	CPC    = 0x0400,
	SBC    = 0x0800,
	ADD    = 0x0c00,
	CPSE   = 0x1000,
	CP     = 0x1400,
	SUB    = 0x1800,
	ADC    = 0x1c00,
	AND    = 0x2000,
	EOR    = 0x2400,
	OR     = 0x2800,
	MOV    = 0x2c00,
	MUL    = 0x9c00,
MASK_OP5P  = 0xf800,
	IN     = 0xb000,
	OUT    = 0xb800,
	LDS    = 0xa000,
	STS    = 0xa800,
MASK_OP5M  = 0xd208,
	LDDY   = 0x8008,
	LDDZ   = 0x8000,
	STDY   = 0x8208,
	STDZ   = 0x8200,
MASK_OP4   = 0xf000,
	CPI    = 0x3000,
	SBCI   = 0x4000,
	SUBI   = 0x5000,
	ORI    = 0x6000,
	ANDI   = 0x7000,
	LD     = 0x7000,
	ST     = 0x7000,
	RJMP   = 0xc000,
	RCALL  = 0xd000,
	LDI    = 0xe000,
};

bool is_2words(uint16_t op) {
	if (((op & MASK_OP10J) == CALL) ||
	    ((op & MASK_OP10J) == JMP)  ||
	    ((op & MASK_OP11) == LDS32) ||
	    ((op & MASK_OP11) == STS32)) {
		return true;
	}
	return false;
}

uint8_t clearb(uint8_t r, int b) {
	return r & ~(1 << b);
}

uint8_t setb(uint8_t r, int b) {
	return r | (1 << b);
}

bool bitn(uint16_t r, int n) {
	return r & (1 << n);
}

bool carryn(uint8_t rr, uint8_t r0, uint8_t r1, int n) {
	return (bitn(r0, n) && bitn(r1, n)) ||
	       (bitn(r1, n) && ! bitn(rr, n)) ||
	       (! bitn(rr, n) && bitn(r0, n));
}

bool overflown(uint8_t rr, uint8_t r0, uint8_t r1, int n) {
	return (bitn(r0, n) && bitn(r1, n) && ! bitn(rr, n)) ||
	       (! bitn(r0, n) && ! bitn(r1, n) && bitn(rr, n));
}

bool borrown(uint8_t rr, uint8_t r0, uint8_t r1, int n) {
	return (! bitn(r0, n) && bitn(r1, n)) ||
	       (bitn(r1, n) && bitn(rr, n)) ||
	       (bitn(rr, n) && ! bitn(r0, n));
}

}

void avr::_nop() {
	disas("nop");

	pc++;
	cycles++;
}

void avr::_movw(reg rd, reg rr) {
	disas("movw\tr%d:r%d, r%d:r%d", rd+1, rd, rr+1, rr);

	regs[rd] = regs[rr];
	regs[rd+1] = regs[rr+1];

	pc++; cycles++;
}

void avr::_muls(reg rd, reg rr) {
	disas("muls\tr%d, r%d", rd, rr);

	int16_t R = (int8_t)regs[rd] * (int8_t)regs[rr];

	sreg.c = bitn(R, 15);
	sreg.z = (R == 0);

	regs[0] = (R & 0x0000ffff) >> 0;
	regs[1] = (R & 0xffff0000) >> 8;

	pc++; cycles += 2;
}

void avr::_mulsu(reg rd, reg rr) {
	disas("muls\tr%d, r%d", rd, rr);

	int16_t R = (int8_t)regs[rd] * regs[rr];

	sreg.c = bitn(R, 15);
	sreg.z = (R == 0);

	regs[0] = (R & 0x0000ffff) >> 0;
	regs[1] = (R & 0xffff0000) >> 8;

	pc++; cycles += 2;
}

void avr::_fmul(reg rd, reg rr) {
	disas("fmul\tr%d, r%d", rd, rr);
	
	uint16_t R = (regs[rd] * regs[rr]) << 1;

	sreg.c = bitn(R, 16);
	sreg.z = (R == 0);

	regs[0] = (R & 0x0000ffff) >> 0;
	regs[1] = (R & 0xffff0000) >> 8;

	pc++; cycles += 2;
}

void avr::_fmuls(reg rd, reg rr) {
	disas("fmuls\tr%d, r%d", rd, rr);
	
	int16_t R = ((int8_t)regs[rd] * (int8_t)regs[rr]) << 1;

	sreg.c = bitn(R, 16);
	sreg.z = (R == 0);

	regs[0] = (R & 0x0000ffff) >> 0;
	regs[1] = (R & 0xffff0000) >> 8;

	pc++; cycles+=2;
}

void avr::_fmulsu(reg rd, reg rr) {
	disas("fmulsu\tr%d, r%d", rd, rr);
	
	int16_t R = ((int8_t)regs[rd] * regs[rr]) << 1;

	sreg.c = bitn(R, 16);
	sreg.z = (R == 0);

	regs[0] = (R & 0x0000ffff) >> 0;
	regs[1] = (R & 0xffff0000) >> 8;

	pc++; cycles+=2;
}

void avr::_cpc(reg rd, reg rr) {
	disas("cpc\tr%d, r%d", rd, rr);
	
	uint8_t R = regs[rd] - regs[rr] - sreg.c;

	sreg.h = borrown(R, regs[rd], regs[rr], 3);
	sreg.n = bitn(regs[rd], 7);
	sreg.v = overflown(R, regs[rd], regs[rr], 7);
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0) && sreg.z;
	sreg.c = borrown(R, regs[rd], regs[rr], 7);

	pc++; cycles++;
}

void avr::_sbc(reg rd, reg rr) {
	disas("sbc\tr%d, r%d", rd, rr);

	uint8_t R = regs[rd] - regs[rr] - sreg.c;

	sreg.h = borrown(R, regs[rd], regs[rr], 3);
	sreg.v = overflown(R, regs[rd], regs[rr], 7);
	sreg.n = bitn(R, 7);
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0) && sreg.z;
	sreg.c = borrown(R, regs[rd], regs[rr], 7);

	regs[rd] = R;

	pc++; cycles+=2;
}

void avr::_add(reg rd, reg rr) {
	disas("add\tr%d, r%d", rd, rr);

	uint8_t R = regs[rd] + regs[rr];

	sreg.h = carryn(R, regs[rd], regs[rr], 3);
	sreg.v = overflown(R, regs[rd], regs[rr], 7);
	sreg.n = bitn(R, 7);
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0);
	sreg.c = carryn(R, regs[rd], regs[rr], 7);

	regs[rd] = R;

	pc++; cycles++;
}

void avr::_cpse(reg rd, reg rr) {
	disas("cpse\tr%d, r%d", rd, rr);

	if (regs[rd] == regs[rr]) {
		uint16_t op;
		pc++; cycles++;
		
		mem.get(pc, &op);
		if (is_2words(op)) {
			pc++; cycles++;
		}
	}

	pc++; cycles++;
}

void avr::_cp(reg rd, reg rr) {
	disas("cp\tr%d, r%d", rd, rr);

	uint8_t R = regs[rd] - regs[rr];

	sreg.h = borrown(R, regs[rd], regs[rr], 3);
	sreg.n = bitn(regs[rd], 7);
	sreg.v = overflown(R, regs[rd], regs[rr], 7);
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0);
	sreg.c = borrown(R, regs[rd], regs[rr], 7);

	pc++; cycles++;
}

void avr::_sub(reg rd, reg rr) {
	disas("sub\tr%d, r%d", rd, rr);
	
	uint8_t R = regs[rd] - regs[rr];

	sreg.h = borrown(R, regs[rd], regs[rr], 3);
	sreg.v = overflown(R, regs[rd], regs[rr], 7);
	sreg.n = bitn(R, 7);
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0);
	sreg.c = borrown(R, regs[rd], regs[rr], 7);

	regs[rd] = R;

	pc++; cycles++;
}

void avr::_adc(reg rd, reg rr) {
	disas("adc\tr%d, r%d", rd, rr);

	uint8_t R = regs[rd] + regs[rr] + sreg.c;

	sreg.h = carryn(R, regs[rd], regs[rr], 3);
	sreg.v = overflown(R, regs[rd], regs[rr], 7);
	sreg.n = bitn(R, 7);
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0);
	sreg.c = carryn(R, regs[rd], regs[rr], 7);

	regs[rd] = R;

	pc++; cycles++;
}

void avr::_and(reg rd, reg rr) {
	disas("and\tr%d, r%d", rd, rr);
	
	uint8_t R = regs[rd] & regs[rr];

	sreg.v = false;
	sreg.n = bitn(R, 7);
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0);

	regs[rd] = R;

	pc++; cycles++;
}

void avr::_eor(reg rd, reg rr) {
	disas("eor\tr%d, r%d", rd, rr);

	uint8_t R = regs[rd] | regs[rr];

	sreg.v = false;
	sreg.n = bitn(R, 7);
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0);

	regs[rd] = R;

	pc++; cycles++;
}

void avr::_or(reg rd, reg rr) {
	disas("or\tr%d, r%d", rd, rr);

	uint8_t R = regs[rd] | regs[rr];

	sreg.n = bitn(R, 7);
	sreg.v = false;
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0);

	regs[rd] = R;

	pc++; cycles++;
}

void avr::_mov(reg rd, reg rr) {
	disas("mov\tr%d, r%d", rd, rr);

	regs[rd] = regs[rr];

	pc++; cycles++;
}

void avr::_cpi(reg rd, uint8_t k) {
	disas("cpi\tr%d, 0x%.2x", rd, k);

	uint8_t R = regs[rd] - k;

	sreg.h = borrown(R, regs[rd], k, 3);
	sreg.v = overflown(R, regs[rd], k, 7);
	sreg.n = bitn(R, 7);
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0);
	sreg.c = borrown(R, regs[rd], k, 7);

	pc++; cycles++;
}

void avr::_sbci(reg rd, uint8_t k) {
	disas("sbci\tr%d, 0x%x", rd, k);
	
	uint8_t R = regs[rd] - k - sreg.c;

	sreg.h = borrown(R, regs[rd], k, 3);
	sreg.v = overflown(R, regs[rd], k, 7);
	sreg.n = bitn(R, 7);
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0) && sreg.z;
	sreg.c = borrown(R, regs[rd], k, 7);

	regs[rd] = R;

	pc++; cycles++;
}


void avr::_subi(reg rd, uint8_t k) {
	disas("subi\tr%d, 0x%x", rd, k);
	
	uint8_t R = regs[rd] - k;

	sreg.h = borrown(R, regs[rd], k, 3);
	sreg.v = overflown(R, regs[rd], k, 7);
	sreg.n = bitn(R, 7);
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0);
	sreg.c = borrown(R, regs[rd], k, 7);

	regs[rd] = R;

	pc++; cycles++;
}

void avr::_ori(reg rd, uint8_t k) {
	disas("subi\tr%d, 0x%x", rd, k);

	uint8_t R = regs[rd] | k;

	sreg.n = bitn(R, 7);
	sreg.v = false;
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0);

	regs[rd] = R;

	pc++; cycles++;
}

void avr::_andi(reg rd, uint8_t k) {
	disas("andi\tr%d, 0x%x", rd, k);

	uint8_t R = regs[rd] & k;

	sreg.v = false;
	sreg.n = bitn(R, 7);
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0);

	regs[rd] = R;

	pc++; cycles++;
}

void avr::_ld(reg rd, ireg ir, int inc) {
	disas("ld\tr%d, %s%s%s", rd,
	                        (inc == -1)?"-":"",
	                        (ir == X)? "X": (ir == Y)? "Y" : "Z",
	                        (inc == +1)?"+":"");

	if (inc == -1) {
		iregs[ir] -= 1;
		cycles++;
	}

	mem.get(iregs[ir], &regs[rd]);

	if (inc == +1) {
		iregs[ir] += 1;
		cycles++;
	}

	pc++; cycles++;
}

void avr::_st(ireg ir, reg rr, int inc) {
	disas("st\t%s%s%s, r%d", (inc == -1)?"-":"",
	                        (ir == X)? "X": (ir == Y)? "Y" : "Z",
	                        (inc == +1)?"+":"",
	                        rr);

	if (inc == -1) {
		iregs[ir] -= 1;
		cycles++;
	}

	mem.set(iregs[ir], regs[rr]);

	if (inc == +1) {
		iregs[ir] += 1;
		cycles++;
	}

	pc++; cycles++;
}

void avr::_ldd(reg rd, ireg ir, uint8_t k) {
	disas("ldd\tr%d, %s+%d", rd,
	                        (ir == X)? "X": (ir == Y)? "Y" : "Z",
	                        k);

	mem.get(iregs[ir]+k, &regs[rd]);

	pc++; cycles++;
}

void avr::_std(ireg ir, reg rr, uint8_t k) {
	disas("std\t%s+%d, r%d", (ir == X)? "X": (ir == Y)? "Y" : "Z",
	                        k,
	                        rr);

	mem.set(iregs[ir]+k, regs[rr]);

	pc++; cycles++;
}

void avr::_pop(reg rr) {
	disas("pop\tr%d", rr);

	if (sp == 0xffff) {
		throw fault();
	}

	sp++;
	mem.get(pc, &regs[rr]);

	pc++; cycles+=2;
}

void avr::_push(reg rr) {
	disas("push\tr%d", rr);

	mem.set(pc, regs[rr]);
	sp--;

	pc++; cycles+=2;
}

void avr::_lds(reg rd) {
	uint16_t k;
	mem.get((pc+1) << 1, &k, 2);

	disas("lds\tr%d, 0x%x", rd, k);
	
	mem.get(k, &regs[rd]);

	pc+=2; cycles+=2;
}

void avr::_lds(reg rd, uint8_t k) {
	disas("lds\tr%d, 0x%x", rd, k);

	mem.get(k, &regs[rd]);

	pc++; cycles++;
}

void avr::_sts(reg rr) {
	uint16_t k;
	mem.get((pc+1) << 1, &k, 2);

	disas("sts\t0x%x, r%d", rr, k);
	
	mem.set(k, regs[rr]);

	pc+=2; cycles+=2;
}

void avr::_sts(reg rr, uint8_t k) {
	disas("sts\t0x%x, r%d", rr, k);

	mem.set(k, regs[rr]);

	pc++; cycles++;
}

void avr::_lpm(reg rd, int inc) {
	disas("lpm\tr%d, Z%s", rd, (inc == +1)?"+":"");

	mem.get(iregs[Z], &regs[rd]);
	if (inc == +1) {
		iregs[Z]++;
	}

	pc++; cycles+=3;
}

void avr::_elpm(reg rd, int inc) {
	disas("elpm\tr%d, Z%s", rd, (inc == +1)?"+":"");

	throw unimplemented();
}

void avr::_jmp(uint32_t h) {
	uint16_t k;
	mem.get((pc+1) << 1, &k, 2);
	h = (h << 16) + k;

	disas("jmp\t0x%x", h << 1);

	pc = h;

	cycles+=3;
}

void avr::_call(uint32_t h) {
	uint16_t k;
	mem.get((pc+1) << 1, &k, 2);
	h = (h << 16) + k;

	disas("call\t0x%x", h << 1);

	pc+=2; sp--;
	mem.set(sp, pc);
	sp--;
	pc = h;

	cycles+=4;
}

void avr::_com(reg rd) {
	disas("com\tr%d", rd);

	uint8_t R = ~regs[rd];

	sreg.v = false;
	sreg.n = bitn(regs[rd], 7);
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0);
	sreg.c = true;

	regs[rd] = R;

	pc++; cycles++;
}

void avr::_neg(reg rd) {
	disas("neg\tr%d", rd);

	uint8_t R = 0 - regs[rd];

	sreg.h = bitn(R, 3) || bitn(regs[rd], 3);
	sreg.c = (R != 0);
	sreg.n = bitn(R, 7);
	sreg.v = (R == 0x80);
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0);

	regs[rd] = R;

	pc++; cycles++;
}

void avr::_swap(reg rd) {
	disas("swap\tr%d", rd);

	regs[rd] = (regs[rd] & 0x0f) << 4 && (regs[rd] & 0xf0) >> 4;

	pc++; cycles++;
}

void avr::_inc(reg rd) {
	disas("inc\tr%d", rd);

	uint8_t R = regs[rd] + 1;

	sreg.n = bitn(R, 7);
	sreg.v = (R == 0x80);
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0);

	regs[rd] = R;

	pc++; cycles++;
}

void avr::_asr(reg rd) {
	disas("asr\tr%d", rd);
	
	uint8_t R = (regs[rd] & 0x80) | (regs[rd] >> 1);

	sreg.c = bitn(regs[rd], 0);
	sreg.n = bitn(R, 7);
	sreg.v = sreg.n ^ sreg.c;
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0);

	regs[rd] = R;

	pc++; cycles++;
}

void avr::_lsr(reg rd) {
	disas("lsr\tr%d", rd);

	uint8_t R = (regs[rd] >> 1);

	sreg.c = bitn(regs[rd], 0);
	sreg.n = false;
	sreg.v = sreg.n ^ sreg.c;
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0);

	regs[rd] = R;

	pc++; cycles++;
}

void avr::_ror(reg rd) {
	disas("ror\tr%d", rd);

	uint8_t R = (sreg.c << 7) | (regs[rd] >> 1);

	sreg.c = bitn(regs[rd], 0);
	sreg.n = bitn(R, 7);
	sreg.v = sreg.n ^ sreg.c;
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0);

	regs[rd] = R;

	pc++; cycles++;
}

void avr::_dec(reg rd) {
	disas("dec\tr%d", rd);

	uint8_t R = regs[rd] - 1;

	sreg.n = bitn(regs[rd], 7);
	sreg.v = (regs[rd] == 0x80);
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0x7f);

	regs[rd] = R;

	pc++; cycles++;
}

void avr::_mul(reg rd, reg rr) {
	disas("mul\tr%d, r%d", rd, rr);

	uint16_t R = regs[rd] * regs[rr];

	sreg.c = bitn(R, 15);
	sreg.z = (R == 0);

	regs[0] = R & 0x0000ffff;
	regs[1] = (R & 0xffff0000) >> 8;

	pc++; cycles+=2;
}

void avr::_ijmp() {
	disas("ijmp");

	pc = iregs[Z];

	pc++; cycles+=2;
}

void avr::_eijmp() {
	disas("eijmp");

	throw unimplemented();
}

void avr::_sec() {
	disas("sec");

	sreg.c = true;

	pc++; cycles++;
}

void avr::_sez() {
	disas("sez");

	sreg.z = true;

	pc++; cycles++;
}

void avr::_sen() {
	disas("sen");

	sreg.n = true;

	pc++; cycles++;
}

void avr::_sev() {
	disas("sev");

	sreg.v = true;

	pc++; cycles++;
}

void avr::_ses() {
	disas("ses");

	sreg.s = true;

	pc++; cycles++;
}

void avr::_seh() {
	disas("seh");

	sreg.h = true;

	pc++; cycles++;
}

void avr::_set() {
	disas("set");

	sreg.t = true;

	pc++; cycles++;
}

void avr::_sei() {
	disas("sei");

	sreg.i = true;

	pc++; cycles++;
}

void avr::_clc() {
	disas("clc");

	sreg.c = false;

	pc++; cycles++;
}

void avr::_clz() {
	disas("clz");

	sreg.z = false;

	pc++; cycles++;
}

void avr::_cln() {
	disas("cln");

	sreg.n = false;

	pc++; cycles++;
}

void avr::_clv() {
	disas("clv");

	sreg.v = false;

	pc++; cycles++;
}

void avr::_cls() {
	disas("cls");

	sreg.s = false;

	pc++; cycles++;
}

void avr::_clh() {
	disas("clh");

	sreg.h = false;

	pc++; cycles++;
}

void avr::_clt() {
	disas("clt");

	sreg.t = false;

	pc++; cycles++;
}

void avr::_cli() {
	disas("cli");

	sreg.i = false;

	pc++; cycles++;
}

void avr::_icall() {
	disas("icall");

	pc++;
	sp--;
	mem.set(sp, pc);
	sp--;
	pc = iregs[Z];

	cycles+=3;
}

void avr::_eicall() {
	disas("eicall");

	throw unimplemented();
}

void avr::_ret() {
	disas("ret");

	if (sp >= 0xfffe) {
		throw fault();
	}

	sp++;
	mem.get(sp, &pc);
	sp++;

	cycles+=4;
}

void avr::_reti() {
	disas("iret");
	
	if (sp >= 0xfffe) {
		throw fault();
	}

	sp++;
	mem.get(sp, &pc);
	sp++;

	sreg.i = true;

	cycles+=4;
}

void avr::_sleep() {
	disas("sleep");

	throw unimplemented();
}

void avr::_break() {
	disas("break");

	debug();

	pc++;
}

void avr::_wdr() {
	disas("wdr");

	watchdog = 0;

	pc++; cycles++;
}

void avr::_lpm() {
	disas("lpm");

	mem.get(iregs[Z], &regs[r0]);

	pc++; cycles+=3;
}

void avr::_elpm() {
	disas("elpm");

	throw unimplemented();
}

void avr::_spm() {
	disas("spm");

	throw unimplemented();
}

void avr::_adiw(reg rd, uint8_t k) {
	disas("adiw\tr%d:r%d, 0x%x" , rd+1, rd, k);

	uint16_t R = (regs[rd+1] << 8) | regs[rd] + k;

	sreg.v = ! bitn(regs[rd+1], 7) && bitn(R, 15);
	sreg.n = bitn(R, 15);
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0);
	sreg.c = ! bitn(R, 15) && bitn(regs[rd+1], 7);

	regs[rd+1] = R >> 8;
	regs[rd] = R & 0xff;

	pc++; cycles+=2;
}

void avr::_sbiw(reg rd, uint8_t k) {
	disas("sbiw\tr%d:r%d, 0x%x" , rd+1, rd, k);

	uint16_t R = (regs[rd+1] << 8) | regs[rd] - k;

	sreg.v = bitn(regs[rd+1], 7) && ! bitn(R, 15);
	sreg.n = bitn(R, 15);
	sreg.s = sreg.n ^ sreg.v;
	sreg.z = (R == 0);
	sreg.c = bitn(R, 15) && ! bitn(regs[rd+1], 7);

	regs[rd+1] = R >> 8;
	regs[rd] = R & 0xff;

	pc++; cycles+=2;
}

void avr::_cbi(uint8_t port, uint8_t k) {
	disas("cbi 0x%x, %d", port, k);

	throw unimplemented();
}

void avr::_sbic(uint8_t port, uint8_t k) {
	disas("sbic 0x%x, %d", port, k);

	throw unimplemented();
}

void avr::_sbi(uint8_t port, uint8_t k) {
	disas("sbi 0x%x, %d", port, k);

	throw unimplemented();
}

void avr::_sbis(uint8_t port, uint8_t k) {
	disas("sbis 0x%x, %d", port, k);

	throw unimplemented();
}

void avr::_in(reg rd, uint8_t port) {
	disas("in\tr%d, 0x%x", rd, port);

	enum {
		sph = 0x3e,
		spl = 0x3d,
	};

	if (port == sph) {
		regs[rd] = (sp & 0xff00) >> 8;
	}
	else if (port == spl) {
		regs[rd] = sp & 0xff;
	}
	else {
		regs[rd] = io.get(port);
	}

	pc++; cycles++;
}

void avr::_out(uint8_t port, reg rr) {
	disas("out\t0x%x, r%d", port, rr);

	enum {
		sph = 0x3e,
		spl = 0x3d,
	};

	if (port == sph) {
		sp = (sp & 0x00ff) | (regs[rr] << 8);
	}
	else if (port == spl) {
		sp = (sp & 0xff00) | regs[rr];
	}
	else {
		io.set(port, regs[rr]);
	}

	pc++; cycles++;
}

void avr::_rjmp(int16_t offset) {
	disas("rjmp\t.%+d", offset << 1);

	if (is_verbose) {
		if (offset == -1) {
			std::cout << "Infinite loop..." << std::endl;
			core::debug();
		}
	}

	pc += offset + 1;

	cycles+=2;
}

void avr::_rcall(int16_t offset) {
	disas("rcall\t.%+d", offset << 1);

	pc++; sp--;
	mem.set(sp, pc);
	sp--;
	pc += offset;

	cycles+=3;
}

void avr::_ldi(reg rd, uint8_t k) {
	disas("ldi\tr%d, 0x%.2x", rd, k);
	
	regs[rd] = k;

	pc++; cycles++;
}

void avr::_brcs(int8_t offset) {
	disas("brcs\t.%+d", (offset << 1));

	if (sreg.c) {
		pc += offset;
		cycles++;
	}

	pc++; cycles++;
}

void avr::_breq(int8_t offset) {
	disas("breq\t.%+d", (offset << 1));

	if (sreg.z) {
		pc += offset;
		cycles++;
	}

	pc++; cycles++;
}

void avr::_brmi(int8_t offset) {
	disas("brmi\t.%+d", (offset << 1));

	if (sreg.n) {
		pc += offset;
		cycles++;
	}

	pc++; cycles++;
}

void avr::_brvs(int8_t offset) {
	disas("brvs\t.%+d", (offset << 1));

	if (sreg.v) {
		pc += offset;
		cycles++;
	}

	pc++; cycles++;
}

void avr::_brlt(int8_t offset) {
	disas("brlt\t.%+d", (offset << 1));

	if (sreg.s) {
		pc += offset;
		cycles++;
	}

	pc++; cycles++;
}

void avr::_brhs(int8_t offset) {
	disas("brhs\t.%+d", (offset << 1));

	if (sreg.h) {
		pc += offset;
		cycles++;
	}

	pc++; cycles++;
}

void avr::_brts(int8_t offset) {
	disas("brts\t.%+d", (offset << 1));

	if (sreg.t) {
		pc += offset;
		cycles++;
	}

	pc++; cycles++;
}

void avr::_brie(int8_t offset) {
	disas("brie\t.%+d", (offset << 1));

	if (sreg.i) {
		pc += offset;
		cycles++;
	}

	pc++; cycles++;
}

void avr::_brcc(int8_t offset) {
	disas("brcc\t.%+d", (offset << 1));

	if (sreg.c == false) {
		pc += offset;
		cycles++;
	}

	pc++; cycles++;
}

void avr::_brne(int8_t offset) {
	disas("brne\t.%+d", (offset << 1));

	if (sreg.z == false) {
		pc += offset;
		cycles++;
	}

	pc++; cycles++;
}

void avr::_brpl(int8_t offset) {
	disas("brpl\t.%+d", (offset << 1));

	if (sreg.n == false) {
		pc += offset;
		cycles++;
	}

	pc++; cycles++;
}

void avr::_brvc(int8_t offset) {
	disas("brvc\t.%+d", (offset << 1));

	if (sreg.v == false) {
		pc += offset;
		cycles++;
	}

	pc++; cycles++;
}

void avr::_brge(int8_t offset) {
	disas("brge\t.%+d", (offset << 1));

	if (sreg.s == false) {
		pc += offset;
		cycles++;
	}

	pc++; cycles++;}

void avr::_brhc(int8_t offset) {
	disas("brhc\t.%+d", (offset << 1));

	if (sreg.h == false) {
		pc += offset;
		cycles++;
	}

	pc++; cycles++;}

void avr::_brtc(int8_t offset) {
	disas("brtc\t.%+d", (offset << 1));

	if (sreg.t == false) {
		pc += offset;
		cycles++;
	}

	pc++; cycles++;}

void avr::_brid(int8_t offset) {
	disas("brid\t%d", (offset << 1));

	if (sreg.i == false) {
		pc += offset;
		cycles++;
	}

	pc++; cycles++;}

void avr::_bld(reg rd, uint8_t k) {
	disas("bld\tr%d, %d", rd, k);

	if (sreg.t) {
		regs[rd] = setb(regs[rd], k);
	}
	else {
		regs[rd] = clearb(regs[rd], k);
	}

	pc++; cycles++;
}

void avr::_bst(reg rd, uint8_t k) {
	disas("bst\tr%d, %d", rd, k);

	sreg.t = bitn(regs[rd], k);

	pc++; cycles++;
}

void avr::_sbrc(reg rd, uint8_t k) {
	disas("sbrc\tr%d, %d", rd, k);

	if (bitn(regs[rd], k) == false) {
		uint16_t op;
		mem.get(pc+1, &op);

		pc++; cycles++;
		if (is_2words(op)) {
			pc++; cycles++;
		}
	}

	pc++; cycles++;
}

void avr::_sbrs(reg rd, uint8_t k) {
	disas("sbrs\tr%d, %d", rd, k);

	if (bitn(regs[rd], k) == true) {
		uint16_t op;
		mem.get(pc+1, &op);

		pc++; cycles++;
		if (is_2words(op)) {
			pc++; cycles++;
		}
	}

	pc++; cycles++;
}

// instruction decoding

avr::reg avr::_2d(uint16_t op) {
	return static_cast<reg>(24 + (((op & 0x30) >> 4) << 1));
}

avr::reg avr::_3d(uint16_t op) {
	return static_cast<reg>(16 + ((op & 0x70) >> 4));
}

avr::reg avr::_3r(uint16_t op) {
	return static_cast<reg>(16 + ((op & 0x07) >> 0));
}

avr::reg avr::_4d(uint16_t op) {
	return static_cast<reg>(16 + ((op & 0xf0) >> 4));
}

avr::reg avr::_4dl(uint16_t op) {
	return static_cast<reg>(((op & 0xf0) >> 4) << 1);
}

avr::reg avr::_4r(uint16_t op) {
	return static_cast<reg>(16 + (op & 0x0f));
}

avr::reg avr::_4rl(uint16_t op) {
	return static_cast<reg>((op & 0x0f) << 1);
}

avr::reg avr::_5d(uint16_t op) {
	return static_cast<reg>((op & 0x1f0) >> 4);
}

avr::reg avr::_5r(uint16_t op) {
	return static_cast<reg>(((op & 0x200) >> 5) + (op & 0xf));
}

uint8_t avr::_5p(uint16_t op) {
	return (op & 0xf8) >> 3;
}

uint8_t avr::_6p(uint16_t op) {
	return ((op & 0x600) >> 5) + (op & 0xf);
}

uint8_t avr::_6q(uint16_t op) {
	return ((op & 0x2000) >> 8) + ((op & 0xc00) >> 7) + (op & 0x7);
}

uint8_t avr::_3k(uint16_t op) {
	return op & 0x7;
}

uint8_t avr::_6k(uint16_t op) {
	return ((op & 0xc0) >> 2) + (op & 0xf);
}

uint8_t avr::_7k(uint16_t op) {
	return ((op & 0xc0) >> 2) + (op & 0xf);
}

uint8_t avr::_8k(uint16_t op) {
	return ((op & 0xf00) >> 4) + (op & 0xf);
}

uint8_t avr::_6h(uint16_t op) {
	return ((op & 0x1f0) >> 3) + (op & 0x1);
}

bool avr::_1i(uint16_t op) {
	return op & 0x1;
}

int8_t avr::_7o(uint16_t op) {
	return ((op & 0x200) >> 2) | ((op & 0x3f8) >> 3);
}

int16_t avr::_12o(uint16_t op) {
	return ((op&0x800)? 0xf000 : 0) + (op & 0xfff);
}

void avr::step() {
	uint16_t op;

	mem.get(pc << 1, &op);

	if (is_verbose) {
		printf("0x%04x: %02x %02x\t", pc<<1, op&0xff, (op>>8)&0xff);
	}

	switch (op & MASK_OP16) {
		case NOP:    _nop(); goto done;
		case IJMP:   _ijmp(); goto done;
		case EIJMP:  _eijmp(); goto done;
		case SEC:    _sec(); goto done;
		case SEZ:    _sez(); goto done;
		case SEN:    _sen(); goto done;
		case SEV:    _sev(); goto done;
		case SES:    _ses(); goto done;
		case SEH:    _seh(); goto done;
		case SET:    _set(); goto done;
		case SEI:    _sei(); goto done;
		case CLC:    _clc(); goto done;
		case CLZ:    _clz(); goto done;
		case CLN:    _cln(); goto done;
		case CLV:    _clv(); goto done;
		case CLS:    _cls(); goto done;
		case CLH:    _clh(); goto done;
		case CLT:    _clt(); goto done;
		case CLI:    _cli(); goto done;
		case ICALL:  _icall(); goto done;
		case EICALL: _eicall(); goto done;
		case RET:    _ret(); goto done;
		case RETI:   _reti(); goto done;
		case SLEEP:  _sleep(); goto done;
		case BREAK:  _break(); goto done;
		case WDR:    _wdr(); goto done;
		case LPM0:   _lpm(); goto done;
		case ELPM0:  _elpm(); goto done;
		case SPM:    _spm(); goto done;
	}

	switch (op & MASK_OP11) {
		case POP:   _pop(_5d(op)); goto done;
		case PUSH:  _push(_5d(op)); goto done;
		case LDS32: _lds(_5d(op)); goto done;
		case STS32: _sts(_5d(op)); goto done;
		case COM:   _com(_5d(op)); goto done;
		case NEG:   _neg(_5d(op)); goto done;
		case SWAP:  _swap(_5d(op)); goto done;
		case INC:   _inc(_5d(op)); goto done;
		case ASR:   _asr(_5d(op)); goto done;
		case LSR:   _lsr(_5d(op)); goto done;
		case ROR:   _ror(_5d(op)); goto done;
		case DEC:   _dec(_5d(op)); goto done;
		case LDX:   _ld(_5d(op), X,  0); goto done;
		case LDXI:  _ld(_5d(op), X, +1); goto done;
		case LDXD:  _ld(_5d(op), X, -1); goto done;
		case LDY:   _ld(_5d(op), Y,  0); goto done;
		case LDYI:  _ld(_5d(op), Y, +1); goto done;
		case LDYD:  _ld(_5d(op), Y, -1); goto done;
		case LDZ:   _ld(_5d(op), Z,  0); goto done;
		case LDZI:  _ld(_5d(op), Z, +1); goto done;
		case LDZD:  _ld(_5d(op), Z, -1); goto done;
		case STX:   _st(X, _5d(op),  0); goto done;
		case STXI:  _st(X, _5d(op), +1); goto done;
		case STXD:  _st(X, _5d(op), -1); goto done;
		case STY:   _st(Y, _5d(op),  0); goto done;
		case STYI:  _st(Y, _5d(op), +1); goto done;
		case STYD:  _st(Y, _5d(op), -1); goto done;
		case STZ:   _st(Z, _5d(op),  0); goto done;
		case STZI:  _st(Z, _5d(op), +1); goto done;
		case STZD:  _st(Z, _5d(op), -1); goto done;
	}

	switch (op & MASK_OP10M) {
		case MULSU:  _mulsu(_3d(op), _3r(op)); goto done;
		case FMUL:   _fmul(_3d(op), _3r(op)); goto done;
		case FMULS:  _fmuls(_3d(op), _3r(op)); goto done;
		case FMULSU: _fmulsu(_3d(op), _3r(op)); goto done;
	}

	switch (op & MASK_OP10J) {
		case LPM:  _lpm(_5d(op), _1i(op)); goto done;
		case ELPM: _elpm(_5d(op), _1i(op)); goto done;
		case JMP:  _jmp(_6h(op)); goto done;
		case CALL: _call(_6h(op)); goto done;
	}

	switch (op & MASK_OP9) {
		case BRCS: _brcs(_7o(op)); goto done;
		case BREQ: _breq(_7o(op)); goto done;
		case BRMI: _brmi(_7o(op)); goto done;
		case BRVS: _brvs(_7o(op)); goto done;
		case BRLT: _brlt(_7o(op)); goto done;
		case BRHS: _brhs(_7o(op)); goto done;
		case BRTS: _brts(_7o(op)); goto done;
		case BRIE: _brie(_7o(op)); goto done;
		case BRCC: _brcc(_7o(op)); goto done;
		case BRNE: _brne(_7o(op)); goto done;
		case BRPL: _brpl(_7o(op)); goto done;
		case BRVC: _brvc(_7o(op)); goto done;
		case BRGE: _brge(_7o(op)); goto done;
		case BRHC: _brhc(_7o(op)); goto done;
		case BRTC: _brtc(_7o(op)); goto done;
		case BRID: _brid(_7o(op)); goto done;
	}

	switch (op & MASK_OP8L) {
		case MOVW: _movw(_4dl(op), _4rl(op)); goto done;
		case MULS: _muls(_4d(op), _4r(op)); goto done;
		case ADIW: _adiw(_2d(op), _6k(op)); goto done;
		case SBIW: _sbiw(_2d(op), _6k(op)); goto done;
		case CBI:  _cbi(_5p(op), _3k(op)); goto done;
		case SBI:  _sbi(_5p(op), _3k(op)); goto done;
		case SBIC: _sbic(_5p(op), _3k(op)); goto done;
		case SBIS: _sbis(_5p(op), _3k(op)); goto done;
	}

	switch (op & MASK_OP8B) {
		case BLD:  _bld(_5d(op), _3k(op)); goto done;
		case BST:  _bst(_5d(op), _3k(op)); goto done;
		case SBRC: _sbrc(_5d(op), _3k(op)); goto done;
		case SBRS: _sbrs(_5d(op), _3k(op)); goto done;
	}

	switch (op & MASK_OP6) {
		case CPC:  _cpc(_5d(op), _5r(op)); goto done;
		case SBC:  _sbc(_5d(op), _5r(op)); goto done;
		case ADD:  _add(_5d(op), _5r(op)); goto done;
		case CPSE: _cpse(_5d(op), _5r(op)); goto done;
		case CP:   _cp(_5d(op), _5r(op)); goto done;
		case SUB:  _sub(_5d(op), _5r(op)); goto done;
		case ADC:  _adc(_5d(op), _5r(op)); goto done;
		case AND:  _and(_5d(op), _5r(op)); goto done;
		case EOR:  _eor(_5d(op), _5r(op)); goto done;
		case OR:   _or(_5d(op), _5r(op)); goto done;
		case MOV:  _mov(_5d(op), _5r(op)); goto done;
		case MUL:  _mul(_5d(op), _5r(op)); goto done;
	}

	switch (op & MASK_OP5P) {
		case IN:  _in (_5d(op), _6p(op)); goto done;
		case OUT: _out(_6p(op), _5d(op)); goto done;
		case LDS: _lds(_4d(op), _7k(op)); goto done;
		case STS: _sts(_4d(op), _7k(op)); goto done;
	}

	switch (op & MASK_OP5M) {
		case LDDY: _ldd(_5d(op), Y, _6q(op)); goto done;
		case LDDZ: _ldd(_5d(op), Z, _6q(op)); goto done;
		case STDY: _std(Y, _5d(op), _6q(op)); goto done;
		case STDZ: _std(Z, _5d(op), _6q(op)); goto done;
	}

	switch (op & MASK_OP4) {
		case CPI:   _cpi(_4d(op), _8k(op)); goto done;
		case SBCI:  _sbci(_4d(op), _8k(op)); goto done;
		case SUBI:  _subi(_4d(op), _8k(op)); goto done;
		case ORI:   _ori(_4d(op), _8k(op)); goto done;
		case ANDI:  _andi(_4d(op), _8k(op)); goto done;
		case LDI:   _ldi(_4d(op), _8k(op)); goto done;
		case RJMP:  _rjmp(_12o(op)); goto done;
		case RCALL: _rcall(_12o(op)); goto done;
	}

	throw illegal();

done:
	return;
}

}
