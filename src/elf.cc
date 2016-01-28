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

#include "elf.h"

#include <stdexcept>
 
namespace insn {

namespace {

#pragma mark ELF

struct elf_header {
	struct {
		uint8_t magic[4];
		uint8_t fclass;
		uint8_t data;
		uint8_t version;
		uint8_t pad[9];
	} ident;
	uint16_t type;
	uint16_t machine;
	uint32_t version;
	uint32_t entry;
	uint32_t phoff;
	uint32_t shoff;
	uint32_t flags;
	uint16_t ehsize;
	uint16_t phentsize;
	uint16_t phnum;
	uint16_t shentsize;
	uint16_t shnum;
	uint16_t shstrndx;
};

struct program_header {
	uint32_t type;
	uint32_t offset;
	uint32_t vaddr;
	uint32_t paddr;
	uint32_t filesz;
	uint32_t memsz;
	uint32_t flags;
	uint32_t align;
};

}

elf::elf(std::string filename_) : loader(filename) {
	filename = filename_;

	throw std::runtime_error("ELF support not implemented. Yet.");
}

bool elf::check_magic(uint32_t magic) {
	const uint32_t EH_7ELF = 0x7f454c46;
	return magic == EH_7ELF;
}

void elf::load_code() {

}

void elf::parse() {
	const uint8_t ELFMAG[4] = {0x7f, 'E', 'L', 'F'};
	const uint8_t ELFCLASS32 = 1;
	const uint8_t ELFDATA2LSB = 1;
	const uint8_t EVCURRENT = 1;
	const uint8_t ETEXEC = 2;
	const uint8_t EMAVR = 0x53;

	elf_header *eh = (elf_header *)mapping;

	if (fsize < sizeof(elf_header)) {
		throw error("Bad ELF header");
	}

	if (std::memcmp(eh->ident.magic, ELFMAG, sizeof(ELFMAG)) != 0) {
		throw error("Not an ELF image");
	}

	if (eh->ehsize != sizeof(elf_header)) {
		throw error("Bad ELF header");
	}
	
	if (eh->ident.fclass != ELFCLASS32) {
		throw error("Format is not 32-bit");
	}

	if (eh->ident.data != ELFDATA2LSB) {
		throw error("Bad indianness");
	}

	if (eh->ident.version != EVCURRENT) {
		throw error("Bad ELF version");
	}

	if (eh->type != ETEXEC) {
		throw error("Not an executable");
	}

	if (eh->machine != EMAVR) {
		throw error("Bad architecture");
	}

	if (eh->version != EVCURRENT) {
		throw error("Bad ELF version");
	}

	entry = eh->entry;

	if (eh->phoff == 0) {
		throw error("No program header");
	}

	if (eh->phentsize != sizeof(program_header)) {
		throw error("Bad program header");
	}

	size_t phend = eh->phoff;
	phend += eh->phnum * sizeof(program_header);
	if (phend > fsize) {
		throw error("Program headers larger than file");
	}
}

void elf::init_segments(void *address, size_t size) {
	elf_header *eh = (elf_header *)mapping;
	program_header *ph = (program_header *)((uintptr_t)mapping + eh->phoff);

	for (int i = 0; i < eh->phnum; i++, ph++) {
		const uint8_t PTLOAD = 1;
		enum { PF_X = 1, PF_W = 2, PF_R = 4 };

		if (ph->type != PTLOAD) {
			throw error("Only loadable segments supported");
		}

		if (ph->align != 0 && ph->align != 1) {
			throw error("Wrong segment alignment");
		}

		if (ph->paddr + ph->memsz > size) {
			throw error("Not enough memory for segments");
		}

		uint8_t *vaddr = (uint8_t *)address + ph->paddr;
		std::memcpy(vaddr, (uint8_t *)eh + ph->offset, ph->filesz);
		std::memset(vaddr + ph->filesz, 0, ph->memsz - ph->filesz);
	}
}

}
