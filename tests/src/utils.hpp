/* Copyright (C) 2021 Edgar B
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "btu/bsa/detail/archive_data.hpp"
#include "btu/bsa/detail/common.hpp"
#include "doctest/doctest.h"

#include <sstream>

using namespace BethUtil::BSA;

namespace doctest {

template<typename T>
struct StringMaker<std::vector<T>>
{
    static String convert(std::vector<T> const &in)
    {
        std::ostringstream oss;

        oss << "[\n";
        for (auto const &val : in)
            oss << StringMaker<T>::convert(val) << ", ";
        oss << "\n]";

        return oss.str().c_str();
    }
};

template<>
struct StringMaker<BSA>
{
    static String convert(BSA const &in)
    {
        std::string res;
        res += "filesSize:" + std::to_string(in.filesSize_);
        res += "maxSize:" + std::to_string(in.maxSize_);
        res += "type:" + std::to_string(static_cast<uint32_t>(in.type_));
        res += "format:" + std::to_string(static_cast<uint32_t>(in.format_));

        return res.c_str();
    }
};

template<>
struct StringMaker<path>
{
    static String convert(path const &in) { return in.string().c_str(); }
};

template<>
struct StringMaker<string>
{
    static String convert(string const &in) { return StringMaker<path>::convert(in); }
};

} // namespace doctest
