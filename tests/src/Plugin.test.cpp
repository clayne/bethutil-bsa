/* Copyright (C) 2021 Edgar B
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "utils.hpp"

TEST_CASE("Plugin names are correctly parsed")
{
    SUBCASE("Simple (SSE)")
    {
        auto sets = Settings::get(Game::SSE);

        REQUIRE_FALSE(FilePath::make("", sets, FileTypes::Plugin).has_value());
        auto plug = FilePath::make("C:/SomeDir/Requiem.esp", sets, FileTypes::Plugin);
        REQUIRE(plug.has_value());
        CHECK(plug->dir_ == "C:/SomeDir");
        CHECK(plug->name_ == path("Requiem"));
        CHECK(plug->suffix_ == path{});
        CHECK(plug->ext_ == ".esp");
    }
    SUBCASE("Complex (SSE)")
    {
        auto sets = Settings::get(Game::SSE);

        auto plug  = FilePath::make("C:/SomeDir/Requiem - Textures01.bsa", sets, FileTypes::BSA);
        auto plug2 = FilePath::make("C:/SomeDir/Requiem01 - Textures.bsa", sets, FileTypes::BSA);
        REQUIRE(plug.has_value());
        CHECK_EQ(plug->counter_, plug2->counter_); // Digits can be after or before suffix
        CHECK_EQ(plug->dir_, "C:/SomeDir");
        CHECK_EQ(plug->name_, bethutil_bsa_STR("Requiem"));
        CHECK_EQ(plug->suffix_, bethutil_bsa_STR("Textures"));
        CHECK_EQ(plug->counter_.value(), 1);
        CHECK_EQ(plug->ext_, ".bsa");
    }
    SUBCASE("Complex 2 (SSE)")
    {
        auto sets = Settings::get(Game::SSE);
        auto plug = FilePath::make("C:/AnotherSomeDir/Requiem01 - Enhancement - Textures.bsa",
                                   sets,
                                   FileTypes::BSA);

        REQUIRE(plug.has_value());
        CHECK_EQ(plug->dir_, "C:/AnotherSomeDir");
        CHECK_EQ(plug->name_, bethutil_bsa_STR("Requiem01 - Enhancement"));
        CHECK_EQ(plug->suffix_, bethutil_bsa_STR("Textures"));
        CHECK(!plug->counter_.has_value());
        CHECK_EQ(plug->ext_, ".bsa");
    }
    SUBCASE("Complex 3 (SSE)")
    {
        auto sets = Settings::get(Game::SSE);
        auto plug = FilePath::make("C:/AnotherSomeDir/Requiem - Enhancement01.bsa", sets, FileTypes::BSA);

        REQUIRE(plug.has_value());
        CHECK_EQ(plug->dir_, "C:/AnotherSomeDir");
        CHECK_EQ(plug->name_, bethutil_bsa_STR("Requiem - Enhancement"));
        CHECK_EQ(plug->suffix_, bethutil_bsa_STR(""));
        CHECK_EQ(plug->counter_.value(), 1);
        CHECK_EQ(plug->ext_, ".bsa");
    }
}
