/* Copyright (C) 2021 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "btu/bsa/detail/Plugin.hpp"

#include "btu/bsa/detail/Algorithms.hpp"

#include <fstream>

namespace btu::bsa {
FilePath::FilePath(path dir, string name, string suffix, path ext, FileTypes type)
    : dir_(std::move(dir))
    , name_(std::move(name))
    , suffix_(std::move(suffix))
    , ext_(std::move(ext))
    , type_(type)
{
}

std::optional<FilePath> FilePath::make(path const &path, GameSettings const &sets, FileTypes type)
{
    if (fs::is_directory(path))
        return std::nullopt;

    FilePath file(path.parent_path(), path.stem().native(), {}, path.extension(), type);

    if (type == FileTypes::Plugin && !contains(sets.pluginExtensions, file.ext_))
        return std::nullopt;

    if (type == FileTypes::BSA && file.ext_ != sets.extension)
        return std::nullopt;

    file.counter_ = eatDigits(file.name_);
    file.suffix_  = eatSuffix(file.name_, sets);

    if (!file.counter_.has_value())
        file.counter_ = eatDigits(file.name_);

    return file;
}

fs::path FilePath::fullPath() const
{
    return (dir_ / fullName()).replace_extension(ext_);
}

btu::bsa::fs::path FilePath::fullName() const
{
    auto const counter = path(counter_ ? std::to_string(*counter_) : "").native();
    auto const suffix  = suffix_.empty() ? bethutil_bsa_STR("") : suffixSeparator + suffix_;
    return path(name_ + counter + suffix);
}

std::optional<int> FilePath::eatDigits(string &str)
{
    size_t firstDigit = str.length() - 1;
    for (; isdigit(str[firstDigit]); --firstDigit)
        ;
    ++firstDigit;

    if (firstDigit != str.length())
    {
        auto ret = std::stoi(str.substr(firstDigit));
        str.erase(firstDigit);
        return ret;
    }
    return std::nullopt;
}

string FilePath::eatSuffix(string &str, GameSettings const &sets)
{
    auto suffixPos = str.rfind(suffixSeparator);

    if (suffixPos == string::npos)
        return {};

    auto suffix = str.substr(suffixPos + suffixSeparator.length());
    if (suffix != sets.suffix && suffix != sets.textureSuffix)
        return {};

    str.erase(suffixPos);
    return suffix;
}

std::vector<FilePath> listXHelper(path const &folderPath, GameSettings const &sets, FileTypes type)
{
    std::vector<FilePath> res;
    for (auto const &f : fs::directory_iterator(folderPath))
        if (auto file = FilePath::make(f.path(), sets, type))
            res.emplace_back(*file);

    return res;
}

bool isLoaded(FilePath const &bsa, GameSettings const &sets)
{
    return std::any_of(sets.pluginExtensions.cbegin(), sets.pluginExtensions.cend(), [&bsa](auto const &ext) {
        auto b            = bsa;
        b.ext_            = ext;
        bool const exact  = fs::exists(b.fullPath());
        b.suffix_         = string{};
        bool const approx = fs::exists(b.fullPath());
        return exact || approx;
    });
}

std::vector<FilePath> listPlugins(path const &folderPath, GameSettings const &sets)
{
    return listXHelper(folderPath, sets, FileTypes::Plugin);
}

std::vector<FilePath> listBSA(path const &folderPath, GameSettings const &sets)
{
    return listXHelper(folderPath, sets, FileTypes::BSA);
}

FilePath findBSAName(path const &folderPath, GameSettings const &sets, BSAType type)
{
    std::vector<FilePath> plugins = listPlugins(folderPath, sets);

    if (plugins.empty())
        plugins.emplace_back(FilePath(folderPath, folderPath.filename(), {}, ".esp", FileTypes::Plugin));

    path const suffix = [type, &sets] {
        if (type == BSAType::Textures)
            return sets.textureSuffix.value();
        return sets.suffix.value_or("");
    }();

    auto checkPlugin = [&sets, &suffix](FilePath &file) {
        file.ext_    = sets.extension;
        file.suffix_ = suffix;
        return !fs::exists(file.fullPath());
    };

    for (auto &plugin : plugins)
        if (checkPlugin(plugin))
            return plugin;

    FilePath plug                   = plugins.front();
    constexpr uint8_t maxIterations = UINT8_MAX;
    for (plug.counter_ = 0; plug.counter_ < maxIterations; ++(*plug.counter_))
        if (checkPlugin(plug))
            return plug;

    throw std::runtime_error("No btu/bsa/plugin name found after 256 tries.");
}

void cleanDummyPlugins(const btu::bsa::fs::path &folderPath, GameSettings const &sets)
{
    if (!sets.sDummyPlugin.has_value())
        return;
    auto const &dummy = *sets.sDummyPlugin;

    for (auto const &plug : listPlugins(folderPath, sets))
    {
        auto const path = plug.fullPath();
        std::fstream file(path, std::ios::binary | std::ios::in | std::ios::ate);
        // It is safe to evaluate file size, as the embedded dummies are the smallest plugins possible

        if (file && file.tellg() == dummy.size())
        {
            file.close();
            fs::remove(path);
        }
    }
}

void makeDummyPlugins(const fs::path &folderPath, GameSettings const &sets)
{
    if (!sets.sDummyPlugin.has_value())
        return;

    for (auto &&bsa : listBSA(folderPath, sets))
    {
        if (isLoaded(bsa, sets))
            continue;

        bsa.ext_    = sets.pluginExtensions.back();
        bsa.suffix_ = {};
        std::ofstream dummy(bsa.fullPath(), std::ios::out | std::ios::binary);

        auto const dummyBytes = *sets.sDummyPlugin;
        dummy.write(reinterpret_cast<const char *>(dummyBytes.data()), dummyBytes.size());
    }
}

} // namespace btu::bsa
