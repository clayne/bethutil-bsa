/* Copyright (C) 2021 Edgar B
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "btu/bsa/detail/archive_data.hpp"
#include "btu/bsa/detail/common.hpp"
#include "catch2/catch.hpp"

#include <sstream>

using namespace btu::bsa;
namespace Catch {

template<>
struct StringMaker<ArchiveData>
{
    static std::string convert(const ArchiveData &in)
    {
        std::string res = "ArchiveData{";
        res += "filesSize:" + std::to_string(in.files_size()) + ", ";
        res += "maxSize:" + std::to_string(in.max_size()) + ", ";
        res += "type:" + std::to_string(static_cast<uint32_t>(in.type()));
        res += "}";

        return res;
    }
};

template<>
struct StringMaker<Path>
{
    static std::string convert(const Path &in) { return in.string(); }
};

} // namespace Catch