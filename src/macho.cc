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

#include "macho.h"

namespace insn {

namespace {

#pragma mark mach-o format

struct fat_header {
	uint32_t magic;
	uint32_t nfat_arch;
};

struct fat_arch {
	uint32_t cputype;
	uint32_t cpusubtype;
	uint32_t offset;
	uint32_t size;
	uint32_t align;
};

struct mach_header_64 {
	uint32_t magic;
	uint32_t cputype;
	uint32_t cpusubtype;
	uint32_t filetype;
	uint32_t ncmds;
	uint32_t sizeofcmds;
	uint32_t flags;
};

const uint32_t MH_MACHO_64 = 0xfeedfacf;
const uint32_t MH_FAT      = 0xbebafeca;

const uint32_t CPU_TYPE_I386   = 7;
const uint32_t CPU_TYPE_ARM    = 12;
const uint32_t CPU_TYPE_X86_64 = CPU_TYPE_I386 | 0x1000000;
const uint32_t CPU_TYPE_ARM64  = CPU_TYPE_ARM  | 0x1000000;

struct load_command {
	uint32_t cmd;
	uint32_t cmdsize;
};

struct segment_command_64 {
	uint32_t cmd;
	uint32_t cmdsize;
	char segname[16];
	uint64_t vmaddr;
	uint64_t vmsize;
	uint64_t fileoff;
	uint64_t filesize;
	uintptr_t maxprot;
	uintptr_t initprot;
	uint32_t nsects;
	uint32_t flags;
};

struct section_64 {
	char sectname[16];
	char segname[16];
	uint64_t addr;
	uint64_t size;
	uint32_t offset;
	uint32_t align;
	uint32_t reloff;
	uint32_t nreloc;
	uint32_t flags;
	uint32_t reserved1;
	uint32_t reserved2;
};

struct nlist_64 {
	union {
	   uint32_t n_strx;
	} n_un;
	uint8_t n_type;
	uint8_t n_sect;
	uint16_t n_desc;
	uint64_t n_value;
};

}

macho::macho(std::string filename) : loader(filename) {
	uint32_t magic;
	mach_header_64 header;

	file.seekg(0);
	file.read((char*)&magic, sizeof(magic));
	file.seekg(0);
	
	switch (magic) {
		case MH_FAT:
			seek_to_header();
		case MH_MACHO_64:
			file.read((char*)&header, sizeof(header));
			break;
		default:
			throw std::runtime_error("Unrecognized file format.");
	}

	if (header.magic != MH_MACHO_64) {
		throw std::runtime_error("Bad file format.");
	}
}

void macho::seek_to_header() {
	fat_header fat;
	fat_arch farch;

	file.read((char*)&fat, sizeof(fat));

	int nfat_arch = __builtin_bswap32(fat.nfat_arch);
	for (int i = 0; i < nfat_arch; i++) {
		file.read((char*)&farch, sizeof(farch));

		uint32_t cputype = __builtin_bswap32(farch.cputype);

		if (cputype == CPU_TYPE_X86_64) {
			arch = "x64";
			break;
		}
		
		if (cputype == CPU_TYPE_ARM64) {
			arch = "arm64";
			break;
		}
	}

	if (arch.empty()) {
		throw std::runtime_error(
			"No slice for supported architectures.");
	}

	file.seekg(__builtin_bswap32(farch.offset));
}

bool macho::check_magic(uint32_t magic) {
	return (magic == MH_MACHO_64) || (magic == MH_FAT);
}

void macho::load_code() {

}

}
