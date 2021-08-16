/* Copyright (C) 2021 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "btu/bsa/BSACreate.hpp"

#include "btu/bsa/BSAExtract.hpp"
#include "btu/bsa/BSATransform.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "utils.hpp"

#include <chrono>

// Intended for library devs only

/* Pack
 * 
 * Requiem MT : 6s
 * Requiem ST : 33s
 * unglau MT 755s
*/

/* Unpack
 * Requiem MT C++ : 14959ms
 * Requiem MT Delphi : 6236ms
 * Unglau MT Delphi : 1494s
*/

#ifdef bethutil_bsa_INTERNAL_TEST
//const auto dir = R"(F:\Edgar\Downloads\unglaubliche Reise EXTENDED Version SSE DV 2.0 BSA)";
const auto dir
    = R"(E:\Programmes\Mod_Skyrim_SE\Cathedral Assets Optimizer\TESTS\TES5_TO_SSE\BSACreation\INPUT - Copy)";

template<typename Func>
void run(Func const &f, std::string const &name)
{
    using namespace std::chrono;

    auto start = high_resolution_clock::now();
    try
    {
        f();
    }
    catch (std::exception const &e)
    {
        std::cerr << "e:" << e.what() << std::endl;
    }
    auto end  = high_resolution_clock::now();
    auto time = duration_cast<milliseconds>(end - start).count();
    std::cout << name << " took " << time << "ms" << std::endl;
}

TEST_CASE("Create BSA")
{
    auto sets = GameSettings::get(Games::SSE);
    //cleanDummyPlugins(dir, sets);
    makeDummyPlugins(dir, sets);
    run([] { create(dir, true, GameSettings::get(Games::SSE)); }, "create");
    run([] { extractAll(dir, GameSettings::get(Games::SSE)); }, "extract");
    run(
        [] {
            libbsarch::bsa bsa;
            transform(
                R"(E:\Programmes\Mod_Skyrim_SE\Cathedral Assets Optimizer\TESTS\TES5_TO_SSE\BSACreation\INPUT - Copy\Requiem.bsa)",
                "out.bsa",
                [](auto, auto data) { return libbsarch::to_vector(std::move(data)); },
                GameSettings::get(Games::SSE));
        },
        "transform");
}
#endif
