/* Copyright (C) 2021 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "btu/bsa/detail/BSASplit.hpp"

#include "btu/bsa/Games.hpp"
#include "btu/bsa/detail/Algorithms.hpp"
#include "btu/bsa/detail/FileTypes.hpp"
#include "btu/bsa/detail/common.hpp"

#include <deque>

namespace btu::bsa {
bool isAllowedPath(path const &dir, fs::directory_entry const &fileinfo)
{
    bool const isDir = fileinfo.is_directory();

    //Removing files at the root directory, those cannot be packed
    bool const isRoot = fileinfo.path().parent_path() == dir;

    return !isDir && !isRoot;
}

std::vector<BSA> splitBSA(path const &dir,
                          bool compactBSA,
                          GameSettings const &sets,
                          AllowFilePred const &allowFile)
{
    // List to avoid invalidating iterators
    std::deque<BSA> bsas{BSA::getBSA(BSAType::Standard, sets),
                         BSA::getBSA(BSAType::Incompressible, sets),
                         BSA::getBSA(BSAType::Textures, sets)};

    auto standardBsa       = std::ref(bsas[0]);
    auto incompressibleBsa = std::ref(bsas[1]);
    auto texturesBsa       = std::ref(bsas[2]);

    for (auto &p : fs::recursive_directory_iterator(dir))
    {
        if (!allowFile(dir, p) || !isAllowedPath(dir, p))
            continue;

        auto const ft = getFileType(p.path(), dir, sets);
        if (ft != FileTypes::Standard && ft != FileTypes::Texture && ft != FileTypes::Incompressible)
            continue;

        auto *pBSA = ft == FileTypes::Texture ? &texturesBsa : &standardBsa;
        pBSA       = ft == FileTypes::Incompressible ? &incompressibleBsa : pBSA;

        //adding files and sizes to list
        pBSA->get().files_.push_back(p.path());
        pBSA->get().filesSize_ += p.file_size();

        if (pBSA->get().filesSize_ >= pBSA->get().maxSize_)
            *pBSA = bsas.emplace_back(BSA::getBSA(pBSA->get().type_, sets));
    }

    //Removing empty BSAs
    std::vector<BSA> cleanBSAs;
    std::copy_if(bsas.cbegin(), bsas.cend(), std::back_inserter(cleanBSAs), [](BSA const &bsa) {
        return !bsa.files_.empty();
    });

    //Merging BSAs that can be merged
    BSA::mergeBSAs(cleanBSAs, compactBSA);

    return cleanBSAs;
}

} // namespace BethUtil::BSA
