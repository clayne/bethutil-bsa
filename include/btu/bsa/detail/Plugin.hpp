/* Copyright (C) 2021 Edgar B
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include "archive_type.hpp"
#include "btu/bsa/detail/settings.hpp"
#include "common.hpp"

namespace btu::bsa {
class FilePath
{
    static inline const OsString suffixSeparator = bethutil_bsa_STR(" - ");

public:
    FilePath(Path dir, OsString name, OsString suffix, Path ext, FileTypes type);
    static std::optional<FilePath> make(Path const &path, Settings const &sets, FileTypes type);

    Path fullPath() const;
    Path fullName() const;

    Path dir_;
    OsString name_;
    OsString suffix_;
    Path ext_;
    std::optional<uint8_t> counter_;
    FileTypes type_{};

private:
    static std::optional<int> eatDigits(OsString &str);
    static OsString eatSuffix(OsString &str, const Settings &sets);

    explicit FilePath() = default;
};

FilePath findBSAName(Path const &folderPath, Settings const &sets, ArchiveType type);

void cleanDummyPlugins(Path const &folderPath, Settings const &sets);
void makeDummyPlugins(Path const &folderPath, Settings const &sets);
} // namespace btu::bsa
