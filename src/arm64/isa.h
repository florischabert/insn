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

namespace insn {
namespace arm64 {

struct isa {
	virtual void _adr(int rd, int imm) = 0;
	virtual void _adrp(int rd, int imm) = 0;
	virtual void _add(int rd, int rn, int imm, int shift, bool is_64, bool set_flags) = 0;
	virtual void _sub(int rd, int rn, int imm, int shift, bool is_64, bool set_flags) = 0;
	virtual void _and(int rd, int rn, int imms, int immr, bool is_64) = 0;
	virtual void _orr(int rd, int rn, int imms, int immr, bool is_64) = 0;
	virtual void _eor(int rd, int rn, int imms, int immr, bool is_64) = 0;
	virtual void _ands(int rd, int rn, int imms, int immr, bool is_64) = 0;
	virtual void _movn(int rd, int imm, int is_64) = 0;
	virtual void _movz(int rd, int imm, int is_64) = 0;
	virtual void _movk(int rd, int imm, int is_64) = 0;
	virtual void _sbfm(int rd, int rn, int imms, int immr, bool is_64) = 0;
	virtual void _bfm(int rd, int rn, int imms, int immr, bool is_64) = 0;
	virtual void _ubfm(int rd, int rn, int imms, int immr, bool is_64) = 0;
	virtual void _ext(int rd, int rn, int rm, int immr, bool is_64) = 0;
};

}
}

#endif
