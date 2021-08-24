/* Copyright (C) 2021 Edgar B
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "archive_type.hpp"
#include "btu/bsa/detail/settings.hpp"
#include "common.hpp"
#include "plugin.hpp"

#include <vector>

namespace btu::bsa {
class ArchiveData
{
public:
    ArchiveData(const Settings &sets, ArchiveType type);

    bool add_file(Path path, std::optional<uintmax_t> size = std::nullopt);

    ArchiveType get_type() const { return type_; }
    ArchiveVersion get_version() const { return version_; }
    bool empty() const;

    auto begin() { return files_.begin(); }
    auto end() { return files_.end(); }

    Path find_name(const Path &folder, const Settings &sets) const;

    uintmax_t files_size() const { return files_size_; }
    uintmax_t max_size() const { return max_size_; }
    ArchiveType type() const { return type_; }

    ArchiveData &operator+=(const ArchiveData &other);
    ArchiveData operator+(ArchiveData const &other) const;

    auto operator<=>(const ArchiveData &) const = default;

    static constexpr bool merge_different_types    = true;
    static constexpr bool separate_different_types = false;

private:
    uintmax_t files_size_ = 0;
    uintmax_t max_size_   = -1;
    ArchiveType type_     = ArchiveType::Standard;
    ArchiveVersion version_{};
    std::vector<Path> files_;
};
} // namespace btu::bsa
