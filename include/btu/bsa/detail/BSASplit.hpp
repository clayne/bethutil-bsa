/* Copyright (C) 2021 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include "BSA.hpp"
#include "btu/bsa/Games.hpp"

#include <functional>

namespace btu::bsa {
using AllowFilePred = std::function<bool(path const &dir, fs::directory_entry const &fileinfo)>;

bool isAllowedPath(path const &dir, fs::directory_entry const &fileinfo);

std::vector<BSA> splitBSA(path const &dir,
                          bool compactBSA,
                          GameSettings const &sets,
                          AllowFilePred const &allowFile = isAllowedPath);
} // namespace BethUtil::BSA
