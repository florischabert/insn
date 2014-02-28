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

#include "loader.h"

#include "macho.h"
#include "elf.h"

#include <stdexcept>

namespace insn {

std::unique_ptr<loader> loader::for_file(std::string filename) {
	uint32_t magic;

	std::ifstream file(filename, std::ifstream::binary);
	if (!file.is_open()) {
		throw std::runtime_error(std::string("Can't open '") + filename + "'.");
	}
	file.read((char*)&magic, sizeof(magic));
	file.close();

	if (macho::check_magic(magic)) {
		return std::unique_ptr<loader>(new macho(filename));
	}
	if (elf::check_magic(magic)) {
		return std::unique_ptr<loader>(new elf(filename));
	}

	throw std::runtime_error("File type not supported.");
}

loader::loader(std::string filename_) {
	filename = filename_;
	file = std::ifstream(filename, std::ifstream::binary);
	if (!file.is_open()) {
		throw std::runtime_error(std::string("Can't open '") + filename + "'.");
	}
}

void loader::load() {
}

}
