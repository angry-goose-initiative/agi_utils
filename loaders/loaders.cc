/*
 * Copyright (C) 2023-2024 John Jekel
 * Copyright (C) 2023-2024 Nick Chan
 * Copyright (C) 2024      Sam Graham
 * See the LICENSE file at the root of the project for licensing info.
*/

#include "loaders.h"

#include <fstream>
#include <vector>

agi::loaders::Res agi::loaders::elf_32(std::string_view   file_name,
                                       BaseWriteFunctor & write_functor)
{
    std::ifstream file(file_name, std::ios::binary);
    if (!file) {
        return Res::FILE_OPEN_ERROR;
    }

    /*
     * Elf header structure layout and details found on Wikipedia.org
     * https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
     * 
     * We effectively strip the ELF file and only load relevent data into memory by
     *  1) Extracting the file header
     *  2) Navigating to the top of the program header table
     *  3) Identifying all program segments with the "load" flag (PT_LOAD)
     *  4) Navigating to the top of the section header table
     *  5) Identifying all sections with the "program data" flag (SHT_PROGBITS)
     *     that are also contained in load segments
     *  6) Loading only the "program data" inside "load" segments into memory
     *
     * NOTE:
     *  - Program headers refer to program segments
     *  - Section headers refer to program sections
     *  - Program sections are subsets of data in program segments
     */

    // File header contains details about the layout and properties of the file
    struct Elf32FileHeader {
        std::array<uint8_t, 16> e_ident; // NOLINT(*-magic-numbers)
        uint16_t e_type;
        uint16_t e_machine;
        uint32_t e_version;
        uint32_t e_entry;
        uint32_t e_phoff;
        uint32_t e_shoff;
        uint32_t e_flags;
        uint16_t e_ehsize;
        uint16_t e_phentsize;
        uint16_t e_phnum;
        uint16_t e_shentsize;
        uint16_t e_shnum;
        uint16_t e_shstrndx;
    } file_header{};

    // Read in file header
    file.read(reinterpret_cast<char *>(&file_header), sizeof(file_header));

    // Validate file header
    constexpr char const * ELF_SIGNATURE = "\177ELF";
    constexpr uint16_t     ELF_FILE_TYPE_EXEC = 2;
    constexpr uint16_t     ELF_MACHINE_RISCV = 0xF3;
    if (std::memcmp(file_header.e_ident.data(), ELF_SIGNATURE, std::strlen(ELF_SIGNATURE)) != 0 // Not an ELF Signature
        || file_header.e_type != ELF_FILE_TYPE_EXEC // Not an executable file
        || file_header.e_machine != ELF_MACHINE_RISCV // Not a riscv file
    ) {
        return Res::FILE_FORMAT_ERROR;
    }

    // Unsupported features
    constexpr uint8_t ELF_ADDRESS_SIZE_32                   = 1;
    constexpr uint8_t ELF_TWOS_COMPLEMENT_AND_LITTLE_ENDIAN = 1;

    // NOLINTNEXTLINE(*-magic-numbers) TODO(Sam) - Make constants for these indices
    if (file_header.e_ident[5] != ELF_TWOS_COMPLEMENT_AND_LITTLE_ENDIAN
        || file_header.e_ident[4] != ELF_ADDRESS_SIZE_32
    ) {
        return Res::FILE_FORMAT_ERROR;
    }

    // Custom data structure used for filtering and loading data
    struct Elf32Chunk {
        uint32_t offset;
        uint32_t size;
        uint32_t vaddr;
    };

    // Program headers contain information about segments of data in the file
    struct Elf32ProgramHeader {
        uint32_t p_type;
        uint32_t p_offset;
        uint32_t p_vaddr;
        uint32_t p_paddr;
        uint32_t p_filesz;
        uint32_t p_memsz;
        uint32_t p_flags;
        uint32_t p_align;
    } program_header{};

    std::vector<Elf32Chunk> program_chunks;

    file.seekg(file_header.e_phoff, std::ios::beg);
    // Iterate over program header table to find program load segments
    for (uint16_t i = 0; i < file_header.e_shnum; i++) {
        file.read(reinterpret_cast<char *>(&program_header), sizeof(program_header));
        // Cache if program page is of type PT_LOAD
        constexpr uint32_t PT_LOAD = 1;
        if (program_header.p_type == PT_LOAD) {
            program_chunks.push_back({
                .offset = program_header.p_offset,
                .size   = program_header.p_filesz,
                .vaddr  = program_header.p_vaddr
            });
        }
    }

    // Section headers contain information about chunks of data within program segment
    struct Elf32SectionHeader {
        uint32_t sh_name;
        uint32_t sh_type;
        uint32_t sh_flags;
        uint32_t sh_addr;
        uint32_t sh_offset;
        uint32_t sh_size;
        uint32_t sh_link;
        uint32_t sh_info;
        uint32_t sh_addralign;
        uint32_t sh_entsize;
    } section_header{};

    std::vector<Elf32Chunk> section_chunks;

    file.seekg(file_header.e_shoff, std::ios::beg);
    // Iterate over the section header table to identify program data sections
    for (uint16_t i = 0; i < file_header.e_shnum; i++) {
        file.read(reinterpret_cast<char *>(&section_header), sizeof(section_header));
        // Filter out section chunks that shouldn't be loaded into memory
        constexpr uint32_t SHT_PROGBITS = 0x1;
        constexpr uint32_t SHT_INIT_ARRAY = 0xe;
        if ((section_header.sh_type != SHT_PROGBITS)
            && (section_header.sh_type != SHT_INIT_ARRAY)
        ) {
            continue;
        }
        // Cache if section is contained in any PT_LOAD program segment
        for (Elf32Chunk const& chunk : program_chunks) {
            if (
                chunk.offset <= section_header.sh_offset &&
                section_header.sh_offset < (chunk.offset + chunk.size)
            ) {
                section_chunks.push_back({
                    .offset = section_header.sh_offset,
                    .size   = section_header.sh_size,
                    .vaddr  = section_header.sh_addr
                });
                break;
            }
        }
    }

    // Iterate over program data found in load segments
    for (Elf32Chunk const& chunk : section_chunks) {
        file.seekg(chunk.offset, std::ios::beg);
        uint32_t addr = chunk.vaddr;
        // Load section
        while (addr < chunk.vaddr + chunk.size) {
            uint8_t data_word{};
            file.read(reinterpret_cast<char *>(&data_word), sizeof(data_word));
            // Write data to memory
            if (!write_functor(addr, data_word)) {
                return Res::WRITE_ERROR;
            }
            addr += sizeof(data_word);
        }
    }

    return Res::OKAY;
}

agi::loaders::Res agi::loaders::verilog_32(std::string_view   file_name,
                                           BaseWriteFunctor & write_functor)
{
    std::fstream file = std::fstream(file_name);
    if (!file) {
        return Res::FILE_OPEN_ERROR;
    }

    // Read the file token by token
    uint64_t addr = 0;
    std::string token;
    while (file >> token) {
        constexpr uint32_t HEX_CHARS_IN_WORD = 8U;
        constexpr int BASE_16 = 16;
        if (token.at(0) == '@') { // `@` indicates a new address (ASSUMING 32-BIT WORDS)
            std::string const new_addr_str = token.substr(1);
            if (new_addr_str.length() != HEX_CHARS_IN_WORD) {
                // Not formatted correctly (bad address)
                return Res::FILE_FORMAT_ERROR;
            }
            addr = std::stoul(new_addr_str, nullptr, BASE_16);
            addr *= 4; // This is a word address, not a byte address, so multiply by 4
        }
        else { // New data word (32-bit, could be an instruction or data)
            if (token.length() != HEX_CHARS_IN_WORD) {
                // Image file is not formatted correctly (data word is not 8
                // characters long). This is likely an objcopy bug.
                return Res::FILE_FORMAT_ERROR;
            }
            
            // The data word this token represents
            uint32_t const data_word = std::stoul(token, nullptr, BASE_16);

            // FIXME(Nick) - This is wrong right now
            // Write the data word to memory and increment the address to the next word
            for (size_t i = 0; i < 4; ++i) {
                if (!write_functor(addr + i, 1)) {
                    return Res::WRITE_ERROR;
                }
            }
            
            // write_memory(addr, DT_WORD, data_word, access_status);
            addr += 4;
        }
    }
    return Res::OKAY;
}
