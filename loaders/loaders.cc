/*
 * Copyright (C) 2023-2024 John Jekel
 * Copyright (C) 2023-2024 Nick Chan
 * Copyright (C) 2024      Sam Graham
 * See the LICENSE file at the root of the project for licensing info.
*/

#include "loaders.h"

using namespace agi::loaders;

Res elf_32(std::string_view file_name, BaseWriteFunctor & write_functor)
{
    return Res::OKAY;
}
