/**
 * Copyright (c) 2014, Floris Chabert. All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef ARM64_ISA_H__
#define ARM64_ISA_H__

#include <functional>

namespace insn {
namespace arm64 {

#pragma mark registers

namespace reg {

struct gpr {
	int idx;
	bool is_64;
};

struct gpr_x : public gpr {
	gpr_x(int idx_) { idx = idx_; is_64 = true; }
};

struct gpr_w : public gpr {
	gpr_w(int idx_) { idx = idx_; is_64 = false; }
};

gpr& x(int idx);
gpr& w(int idx);

struct neon {
	neon(int idx_) : idx(idx_) {}
	int idx;
};

}

#pragma mark instructions

#define isa_call(instr, args...) \
	std::bind(instr, std::placeholders::_1, args)

struct isa {
	typedef std::function<void(isa*)> instr;
	void exec(instr isa_func);

	virtual void _adr(reg::gpr rd, int imm) = 0;
	virtual void _adrp(reg::gpr rd, int imm) = 0;
	virtual void _add(reg::gpr rd, reg::gpr rn, int imm, int shift) = 0;
	virtual void _adds(reg::gpr rd, reg::gpr rn, int imm, int shift) = 0;
	virtual void _sub(reg::gpr rd, reg::gpr rn, int imm, int shift) = 0;
	virtual void _subs(reg::gpr rd, reg::gpr rn, int imm, int shift) = 0;
	virtual void _and(reg::gpr rd, reg::gpr rn, int imms, int immr) = 0;
	virtual void _orr(reg::gpr rd, reg::gpr rn, int imms, int immr) = 0;
	virtual void _eor(reg::gpr rd, reg::gpr rn, int imms, int immr) = 0;
	virtual void _ands(reg::gpr rd, reg::gpr rn, int imms, int immr) = 0;
	virtual void _movn(reg::gpr rd, int imm) = 0;
	virtual void _movz(reg::gpr rd, int imm) = 0;
	virtual void _movk(reg::gpr rd, int imm) = 0;
	virtual void _sbfm(reg::gpr rd, reg::gpr rn, int imms, int immr) = 0;
	virtual void _bfm(reg::gpr rd, reg::gpr rn, int imms, int immr) = 0;
	virtual void _ubfm(reg::gpr rd, reg::gpr rn, int imms, int immr) = 0;
	virtual void _ext(reg::gpr rd, reg::gpr rn, reg::gpr rm, int immr) = 0;
};

}
}

#endif
