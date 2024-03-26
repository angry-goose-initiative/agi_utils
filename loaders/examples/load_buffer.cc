/*
 * Copyright (C) 2024 Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
*/

#include "../loaders.h"

#include <cstdint>

void load_buffer() {
    // The buffer that we want to load the elf file into
    uint8_t buffer[1024];

    // Provide a write functor with the buffer information
    agi::loaders::BufferWriteFunctor write_functor(buffer, sizeof(buffer));

    // Call the loader
    auto const res = agi::loaders::elf_32("example.elf", write_functor);

    // Check for loader error
    if (res != agi::loaders::Res::OKAY) {
        // Handle error
    }
}
