#ifndef AVR_H
#define AVR_H

#include "core.h"

namespace coresim {

class avr : public core {
public:
	avr(vmem &m, vio &i);
	void step();
	void reset();
	void debug();

	static std::string name;

private:
	uint16_t pc;
	uint16_t sp;

	enum reg {
		r0,  r1,  r2,  r3,  r4,  r5,  r6,  r7,
		r8,  r9,  r10, r11, r12, r13, r14, r15,
		r16, r17, r18, r19, r20, r21, r22, r23,
		r24, r25, r26, r27, r28, r29, r30, r31
	};
	enum ireg { X, Y, Z  };
	union {
		uint8_t regs[32];
		struct {
			uint8_t iregs_r[26];
			uint16_t iregs[3];
		};
	};

	struct {
		bool c, z, n, v, s, h, t, i;
	} sreg;

	uint64_t cycles;

	int watchdog;

	// opcodes
	void _nop();
	void _ijmp();
	void _eijmp();
	void _sec();
	void _sez();
	void _sen();
	void _sev();
	void _ses();
	void _seh();
	void _set();
	void _sei();
	void _clc();
	void _clz();
	void _cln();
	void _clv();
	void _cls();
	void _clh();
	void _clt();
	void _cli();
	void _icall();
	void _eicall();
	void _ret();
	void _reti();
	void _sleep();
	void _break();
	void _wdr();
	void _lpm();
	void _elpm();
	void _spm();
	void _pop(reg rd);
	void _push(reg rd);
	void _lds(reg rd);
	void _lds(reg rd, uint8_t k);
	void _sts(reg rr);
	void _sts(reg rr, uint8_t k);
	void _com(reg rd);
	void _neg(reg rd);
	void _swap(reg rd);
	void _inc(reg rd);
	void _asr(reg rd);
	void _lsr(reg rd);
	void _ror(reg rd);
	void _dec(reg rd);
	void _mulsu(reg rd, reg rr);
	void _fmul(reg rd, reg rr);
	void _fmuls(reg rd, reg rr);
	void _fmulsu(reg rd, reg rr);
	void _lpm(reg rd, int inc);
	void _elpm(reg rd, int inc);
	void _jmp(uint32_t h);
	void _call(uint32_t h);
	void _brcs(int8_t offset);
	void _breq(int8_t offset);
	void _brmi(int8_t offset);
	void _brvs(int8_t offset);
	void _brlt(int8_t offset);
	void _brhs(int8_t offset);
	void _brts(int8_t offset);
	void _brie(int8_t offset);
	void _brcc(int8_t offset);
	void _brne(int8_t offset);
	void _brpl(int8_t offset);
	void _brvc(int8_t offset);
	void _brge(int8_t offset);
	void _brhc(int8_t offset);
	void _brtc(int8_t offset);
	void _brid(int8_t offset);
	void _movw(reg rd, reg rr);
	void _muls(reg rd, reg rr);
	void _adiw(reg rd, uint8_t k);
	void _sbiw(reg rd, uint8_t k);
	void _cbi(uint8_t port, uint8_t k);
	void _sbic(uint8_t port, uint8_t k);
	void _sbi(uint8_t port, uint8_t k);
	void _sbis(uint8_t port, uint8_t k);
	void _bld(reg rd, uint8_t k);
	void _bst(reg rd, uint8_t k);
	void _sbrc(reg rd, uint8_t k);
	void _sbrs(reg rd, uint8_t k);
	void _cpc(reg rd, reg rr);
	void _sbc(reg rd, reg rr);
	void _add(reg rd, reg rr);
	void _cpse(reg rd, reg rr);
	void _cp(reg rd, reg rr);
	void _sub(reg rd, reg rr);
	void _adc(reg rd, reg rr);
	void _and(reg rd, reg rr);
	void _eor(reg rd, reg rr);
	void _or(reg rd, reg rr);
	void _mov(reg rd, reg rr);
	void _mul(reg rd, reg rr);
	void _in(reg rd, uint8_t port);
	void _out(uint8_t port, reg rd);
	void _cpi(reg rd, uint8_t k);
	void _sbci(reg rd, uint8_t k);
	void _subi(reg rd, uint8_t k);
	void _ori(reg rd, uint8_t k);
	void _andi(reg rd, uint8_t k);
	void _ldi(reg rd, uint8_t k);
	void _ld(reg rd, ireg ir, int inc);
	void _st(ireg ir, reg rr, int inc);
	void _ldd(reg rd, ireg ir, uint8_t k);
	void _std(ireg ir, reg rr, uint8_t k);
	void _rjmp(int16_t offset);
	void _rcall(int16_t offset);

	// operands
	reg _2d(uint16_t op);
	reg _3d(uint16_t op);
	reg _3r(uint16_t op);
	reg _4d(uint16_t op);
	reg _4dl(uint16_t op);
	reg _4r(uint16_t op);
	reg _4rl(uint16_t op);
	reg _5d(uint16_t op);
	reg _5r(uint16_t op);
	uint8_t _5p(uint16_t op);
	uint8_t _6p(uint16_t op);
	uint8_t _6q(uint16_t op);
	uint8_t _3k(uint16_t op);
	uint8_t _6k(uint16_t op);
	uint8_t _7k(uint16_t op);
	uint8_t _8k(uint16_t op);
	uint8_t _6h(uint16_t op);
	bool _1i(uint16_t op);
	int8_t _7o(uint16_t op);
	int16_t _12o(uint16_t op);
};

}

#endif
