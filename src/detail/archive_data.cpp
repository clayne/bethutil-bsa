/* Copyright (C) 2021 Edgar B
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "btu/bsa/detail/archive_data.hpp"

#include "btu/bsa/detail/settings.hpp"
#include "btu/common/algorithms.hpp"

namespace btu::bsa {
ArchiveData::ArchiveData(const Settings &sets, ArchiveType type)
    : max_size_(sets.maxSize)
    , type_(type)
{
}

Path ArchiveData::find_name(Path const &folder, Settings const &sets) const
{
    return findBSAName(folder, sets, type_).fullPath();
}

bool ArchiveData::empty() const
{
    return files_size_ == 0;
}

bool ArchiveData::add_file(Path path)
{
    if (files_size_ + fs::file_size(path) > max_size_)
        return false;

    files_size_ += fs::file_size(files_.emplace_back(std::move(path)));
    return true;
}

ArchiveData &ArchiveData::operator+=(const ArchiveData &other)
{
    if (other.version_ != version_)
        throw std::runtime_error("Cannot merge ArchiveData of different format");

    if (files_size_ + other.files_size_ > max_size_)
        throw std::runtime_error("Cannot merge ArchiveData with file size over max size");

    files_size_ += other.files_size_;
    files_.insert(files_.end(), other.files_.begin(), other.files_.end());

    if (type_ == ArchiveType::Incompressible || other.type_ == ArchiveType::Incompressible)
        type_ = ArchiveType::Incompressible;
    else if (type_ != other.type_)
        type_ = ArchiveType::Standard;
    return *this;
}

ArchiveData ArchiveData::operator+(ArchiveData const &other) const
{
    ArchiveData copy = *this;
    copy += other;
    return copy;
}

} // namespace btu::bsa
