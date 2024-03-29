/*
 * Copyright (C) 2024 Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
*/

#include "../loaders.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>

constexpr size_t BUFFER_SIZE = 1024;

int main(int argc, char * argv[])
{
    if (argc != 2) {
        std::cerr << "No file provided to load!" << std::endl;
        return -1;
    }

    // The buffer that we want to load the elf file into
    std::array<uint8_t, BUFFER_SIZE> buffer{};

    // Create write functor with the buffer information
    agi::loaders::BufferWriteFunctor write_functor({buffer.data(), buffer.size()});

    // Call the loader
    auto const res = agi::loaders::elf_32(argv[1], write_functor); // NOLINT(*-pointer-arithmetic)

    // Check for loader error
    if (res != agi::loaders::Res::OKAY) {
        std::cerr << "Loader error occurred!" << std::endl;
        return -1;
    }

    return 0;
}
