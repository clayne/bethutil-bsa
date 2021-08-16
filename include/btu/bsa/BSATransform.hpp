/* Copyright (C) 2021 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

/*
#include "Games.hpp"
#include "bsa.hpp"
#include "common.hpp"
#include "detail/BSACallback.hpp"
#include "detail/Plugin.hpp"
#include "transform_archive.hpp"

namespace BethUtil::BSA {
inline void transform(path const& file,
                      path const& output,
                      libbsarch::transform_callback const& callback,
                      GameSettings const& sets)
{
    auto const path = FilePath::make(file, sets, FileTypes::BSA);
    auto format     = sets.format;
    if (path->suffix_ == sets.textureSuffix)
        format = *sets.textureFormat;

    libbsarch::bsa bsa;

    bsa.set_dds_callback(&BSACallback, file);
    bsa.load(file);

    libbsarch::transform_archive(bsa, output, callback, format);
}
} // namespace BethUtil::BSA
*/
