/* Copyright (C) 2021 Edgar B
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <random>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "btu/bsa/detail/archive_data.hpp"
#include "utils.hpp"

using namespace BethUtil::BSA;

TEST_CASE("Merging BSAs of same type")
{
    auto input = std::vector<BSA>{BSA{1'000'000, 1'000, BSAType::Standard},
                                  BSA{1'000'000, 998'000, BSAType::Standard}};

    auto const expected = std::vector<BSA>{BSA{1'000'000, 999'000, BSAType::Standard}};

    BSA::mergeBSAs(input, BSA::mergeDifferentTypes);

    CHECK_EQ(input, expected);
}

TEST_CASE("Merging BSAs of different types with mergeDifferentTypes enabled")
{
    auto input          = std::vector<BSA>{BSA{1'000'000, 1'000, BSAType::Standard},
                                  BSA{1'000'000, 998'000, BSAType::Textures}};
    auto const expected = std::vector<BSA>{BSA{1'000'000, 999'000, BSAType::Standard}};

    BSA::mergeBSAs(input, BSA::mergeDifferentTypes);

    CHECK_EQ(input, expected);
}

TEST_CASE("Merging BSAs of different types with mergeDifferentTypes disabled")
{
    auto input    = std::vector<BSA>{BSA{1'000'000, 0, BSAType::Standard},
                                  BSA{1'000'000, 0, BSAType::Textures},
                                  BSA{1'000'000, 0, BSAType::Incompressible}};
    auto expected = input;

    BSA::mergeBSAs(input, BSA::separateDifferentTypes);

    //Sorting to allow permutations
    auto sort = [](auto &&left, auto &&right) { return left.filesSize_ < right.filesSize_; };
    std::sort(input.begin(), input.end(), sort);
    std::sort(expected.begin(), expected.end(), sort);

    CHECK_EQ(input, expected);
}

TEST_CASE("Merging lots of BSAs with mergeDifferentTypes disabled")
{
    auto input = std::vector<BSA>{BSA{1'000, 20, BSAType::Textures},
                                  BSA{1'000, 100, BSAType::Textures},
                                  BSA{1'000, 200, BSAType::Textures},
                                  BSA{1'000, 300, BSAType::Textures},
                                  BSA{1'000, 25, BSAType::Standard},
                                  BSA{1'000, 50, BSAType::Standard},
                                  BSA{1'000, 400, BSAType::Standard},
                                  BSA{1'000, 500, BSAType::Standard},
                                  BSA{1'000, 10, BSAType::Incompressible},
                                  BSA{1'000, 50, BSAType::Incompressible},
                                  BSA{1'000, 100, BSAType::Incompressible},
                                  BSA{1'000, 1000, BSAType::Incompressible}};

    auto expected = std::vector<BSA>{BSA{1'000, 975, BSAType::Standard},
                                     BSA{1'000, 620, BSAType::Textures},
                                     BSA{1'000, 1000, BSAType::Incompressible},
                                     BSA{1'000, 160, BSAType::Incompressible}};

    //Shuffling to prevent the function from getting an already ready vector
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(input.begin(), input.end(), g);

    BSA::mergeBSAs(input, BSA::separateDifferentTypes);

    //Sorting to allow permutations
    auto sort = [](auto &&left, auto &&right) { return left.filesSize_ < right.filesSize_; };
    std::sort(input.begin(), input.end(), sort);
    std::sort(expected.begin(), expected.end(), sort);

    CHECK_EQ(input, expected);
}
