/* Copyright (C) 2021 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "btu/bsa/detail/BSA.hpp"

#include "btu/bsa/Games.hpp"
#include "btu/bsa/detail/Algorithms.hpp"
#include "btu/bsa/detail/FileTypes.hpp"

namespace btu::bsa {
BSA BSA::getBSA(BSAType const &type, GameSettings const &sets)
{
    BSA bsa;
    bsa.type_    = type;
    bsa.maxSize_ = sets.maxSize;

    if (type == BSAType::Textures)
    {
        if (!sets.textureFormat.has_value())
        {
            auto const game = std::to_string(static_cast<int>(sets.game));
            throw std::runtime_error("This game does not support texture bsa: " + game);
        }

        bsa.format_ = *sets.textureFormat;
    }
    else
    {
        bsa.format_ = sets.format;
    }

    return bsa;
}

BSA::BSA(uintmax_t maxSize, uintmax_t size, BSAType type)
    : filesSize_(size)
    , maxSize_(maxSize)
    , type_(type)
{
}

void BSA::name(path const &folder, GameSettings const &sets)
{
    path_ = findBSAName(folder, sets, type_).fullPath();
}

void BSA::mergeBSAs(std::vector<BSA> &list, bool merge)
{
    auto secondBegin = std::partition(list.begin(), list.end(), [](BSA const &val) {
        return val.type_ == BSAType::Standard;
    });

    auto thirdBegin = std::partition(secondBegin, list.end(), [](BSA const &val) {
        return val.type_ == BSAType::Textures;
    });

    auto sortBSADescending = [](auto &&one, auto &&two) { return one.filesSize_ > two.filesSize_; };
    //BSAs small enough to be merged
    auto notMaxSize = [](auto &&left, auto &&right) {
        return left.filesSize_ + right.filesSize_ < left.maxSize_;
    };

    auto sortMerge = [&sortBSADescending, &notMaxSize](auto begin, auto end) {
        std::sort(begin, end, sortBSADescending);
        return merge_if(begin, end, notMaxSize);
    };

    auto firstEnd  = sortMerge(list.begin(), secondBegin);
    auto secondEnd = sortMerge(secondBegin, thirdBegin);
    auto thirdEnd  = sortMerge(thirdBegin, list.end());

    std::vector<BSA> result;
    result.reserve(list.size());

    std::move(list.begin(), firstEnd, std::back_inserter(result));
    std::move(secondBegin, secondEnd, std::back_inserter(result));
    std::move(thirdBegin, thirdEnd, std::back_inserter(result));

    if (merge)
        result.erase(sortMerge(result.begin(), result.end()), result.end());

    list = result;
}

BSA &BSA::operator+=(BSA const &other)
{
    filesSize_ += other.filesSize_;
    files_.reserve(files_.size() + other.files_.size());
    files_.insert(files_.end(), other.files_.begin(), other.files_.end());

    if (type_ == BSAType::Incompressible || other.type_ == BSAType::Incompressible)
        type_ = BSAType::Incompressible;
    else if (type_ != other.type_)
        type_ = BSAType::Standard;
    return *this;
}

BSA BSA::operator+(BSA const &other) const
{
    BSA copy = *this;
    copy += other;
    return copy;
}

bool BSA::operator==(BSA const &other) const
{
    return path_ == other.path_ && filesSize_ == other.filesSize_ && files_ == other.files_
           && maxSize_ == other.maxSize_ && type_ == other.type_ && format_ == other.format_;
}
} // namespace btu::bsa
