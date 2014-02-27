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

#include "isa.h"

namespace insn {
namespace arm64 {

namespace reg {

gpr& x(int idx) {
	static gpr_x x[] = {
		gpr_x(0),  gpr_x(1),  gpr_x(2),  gpr_x(3),
		gpr_x(4),  gpr_x(5),  gpr_x(6),  gpr_x(7),
		gpr_x(8),  gpr_x(9),  gpr_x(10), gpr_x(11),
		gpr_x(12), gpr_x(13), gpr_x(14), gpr_x(15),
		gpr_x(16), gpr_x(17), gpr_x(18), gpr_x(19),
		gpr_x(20), gpr_x(21), gpr_x(22), gpr_x(23),
		gpr_x(24), gpr_x(25), gpr_x(26), gpr_x(27),
		gpr_x(28), gpr_x(29), gpr_x(30), gpr_x(31)
	};

	return x[idx];
}

gpr& w(int idx) {
	static gpr_w w[] = {
		gpr_w(0),  gpr_w(1),  gpr_w(2),  gpr_w(3),
		gpr_w(4),  gpr_w(5),  gpr_w(6),  gpr_w(7),
		gpr_w(8),  gpr_w(9),  gpr_w(10), gpr_w(11),
		gpr_w(12), gpr_w(13), gpr_w(14), gpr_w(15),
		gpr_w(16), gpr_w(17), gpr_w(18), gpr_w(19),
		gpr_w(20), gpr_w(21), gpr_w(22), gpr_w(23),
		gpr_w(24), gpr_w(25), gpr_w(26), gpr_w(27),
		gpr_w(28), gpr_w(29), gpr_w(30), gpr_w(31)
	};

	return w[idx];
}

}

void isa::exec(instr isa_func) {
	isa_func(this);
}

}
}
