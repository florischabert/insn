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

#ifndef ARM64_PRINTER_H__
#define ARM64_PRINTER_H__

#include "isa.h"

namespace insn {
namespace arm64 {

struct printer : public isa {
	void _adr(reg::gpr rd, int imm);
	void _adrp(reg::gpr rd, int imm);
	void _add(reg::gpr rd, reg::gpr rn, int imm, int shift);
	void _adds(reg::gpr rd, reg::gpr rn, int imm, int shift);
	void _sub(reg::gpr rd, reg::gpr rn, int imm, int shift);
	void _subs(reg::gpr rd, reg::gpr rn, int imm, int shift);
	void _and(reg::gpr rd, reg::gpr rn, int imms, int immr);
	void _orr(reg::gpr rd, reg::gpr rn, int imms, int immr);
	void _eor(reg::gpr rd, reg::gpr rn, int imms, int immr);
	void _ands(reg::gpr rd, reg::gpr rn, int imms, int immr);
	void _movn(reg::gpr rd, int imm);
	void _movz(reg::gpr rd, int imm);
	void _movk(reg::gpr rd, int imm);
	void _sbfm(reg::gpr rd, reg::gpr rn, int imms, int immr);
	void _bfm(reg::gpr rd, reg::gpr rn, int imms, int immr);
	void _ubfm(reg::gpr rd, reg::gpr rn, int imms, int immr);
	void _ext(reg::gpr rd, reg::gpr rn, reg::gpr rm, int immr);
};

}
}

#endif
