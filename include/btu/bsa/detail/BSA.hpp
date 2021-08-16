/* Copyright (C) 2021 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "BSAType.hpp"
#include "Plugin.hpp"
#include "btu/bsa/Games.hpp"
#include "common.hpp"

#include <vector>

namespace btu::bsa {
struct BSA
{
    static BSA getBSA(BSAType const &type, GameSettings const &sets);

    explicit BSA() = default;
    explicit BSA(uintmax_t maxSize, uintmax_t size, BSAType type);

    uintmax_t filesSize_ = 0;
    uintmax_t maxSize_   = -1;
    path path_;
    std::vector<path> files_{};
    BSAType type_ = BSAType::Standard;
    version format_{};

    BSA &operator+=(BSA const &other);
    BSA operator+(BSA const &other) const;
    bool operator==(BSA const &other) const;

    /*!
     * \brief Finds a name for a BSA
     * \param folder The folder in which the BSA will be
     * \param settings BethUtil::BSA Settings
     */
    void name(path const &folder, GameSettings const &sets);

    static constexpr bool mergeDifferentTypes    = true;
    static constexpr bool separateDifferentTypes = false;

    //! \brief Merges BSAs when possible, according to their max size
    static void mergeBSAs(std::vector<BSA> &list, bool merge);
};
} // namespace btu::bsa
