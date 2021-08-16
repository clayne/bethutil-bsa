#pragma once

#include "detail/Algorithms.hpp"
#include "detail/archive.hpp"
#include "detail/common.hpp"

#include <optional>
#include <variant>

namespace btu::bsa {
namespace Dummy {
static constexpr uint8_t Skyrim[] = {0x54, 0x45, 0x53, 0x34, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00,
                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                     0x2B, 0x00, 0x00, 0x00, 0x48, 0x45, 0x44, 0x52, 0x0C, 0x00,
                                     0x9A, 0x99, 0xD9, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
                                     0x00, 0x00, 0x43, 0x4E, 0x41, 0x4D, 0x01, 0x00, 0x00};

static constexpr uint8_t SkyrimSE[] = {0x54, 0x45, 0x53, 0x34, 0x19, 0x00, 0x00, 0x00, 0x00, 0x02,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x2C, 0x00, 0x00, 0x00, 0x48, 0x45, 0x44, 0x52, 0x0C, 0x00,
                                       0x9A, 0x99, 0xD9, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
                                       0x00, 0x00, 0x43, 0x4E, 0x41, 0x4D, 0x01, 0x00, 0x00};

static constexpr uint8_t Oblivion[] = {0x54, 0x45, 0x53, 0x34, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x45,
                                       0x44, 0x52, 0x0C, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x08, 0x00, 0x00, 0x43, 0x4E, 0x41, 0x4D, 0x08, 0x00,
                                       0x44, 0x45, 0x46, 0x41, 0x55, 0x4C, 0x54, 0x00};

static constexpr uint8_t FNV[] = {0x54, 0x45, 0x53, 0x34, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x0F, 0x00, 0x00, 0x00, 0x48, 0x45, 0x44, 0x52, 0x0C, 0x00,
                                  0x1F, 0x85, 0xAB, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
                                  0x00, 0x00, 0x43, 0x4E, 0x41, 0x4D, 0x01, 0x00, 0x00};

static constexpr uint8_t Fallout4[] = {0x54, 0x45, 0x53, 0x34, 0x19, 0x00, 0x00, 0x00, 0x00, 0x02,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x83, 0x00, 0x00, 0x00, 0x48, 0x45, 0x44, 0x52, 0x0C, 0x00,
                                       0x33, 0x33, 0x73, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
                                       0x00, 0x00, 0x43, 0x4E, 0x41, 0x4D, 0x01, 0x00, 0x00};
} // namespace Dummy

enum Games
{
    Oblivion,
    FNV,
    SLE,
    SSE,
    FO4
};

struct AllowedPath
{
    path extension;
    std::vector<path> directories;

    bool checkExtension = true;
    bool checkDirectory = true;

    static auto const inline root = path("root");

    bool check(path const &filepath, path const &root) const;
};

struct GameSettings
{
    Games game;

    std::uintmax_t maxSize;

    version format;
    std::optional<version> textureFormat;

    std::optional<path> suffix;
    std::optional<path> textureSuffix;

    path extension;

    std::vector<path> pluginExtensions;
    std::optional<std::vector<uint8_t>> sDummyPlugin;

    std::vector<AllowedPath> standardFiles;
    std::vector<AllowedPath> textureFiles;
    std::vector<AllowedPath> incompressibleFiles;

    static GameSettings const &get(Games game);
};

inline const GameSettings &GameSettings::get(Games game)
{
    constexpr std::uintmax_t gigabyte = 1024 * 1024 * 1024;

    static GameSettings defaultSets = [=] {
        GameSettings sets;
        sets.game             = SSE;
        sets.maxSize          = static_cast<std::uintmax_t>(gigabyte * 1.7); // Safe
        sets.format           = version::sse;
        sets.textureFormat    = version::sse;
        sets.textureSuffix    = "Textures";
        sets.extension        = ".bsa";
        sets.pluginExtensions = {".esl", ".esm", ".esp"};
        sets.sDummyPlugin     = std::vector(std::begin(Dummy::SkyrimSE), std::end(Dummy::SkyrimSE));
        sets.standardFiles    = {
            AllowedPath{".bgem", {"materials"}},
            AllowedPath{".bgsm", {"materials"}},
            AllowedPath{".bto", {"meshes"}},
            AllowedPath{".btr", {"meshes"}},
            AllowedPath{".btt", {"meshes"}},
            AllowedPath{".dlodsettings", {"lodsettings"}},
            AllowedPath{".dtl", {"meshes"}}, // Unsure
            AllowedPath{".egm", {"meshes"}}, // Unsure
            AllowedPath{".jpg", {"root"}},
            AllowedPath{".hkx", {"meshes"}},
            AllowedPath{".lst", {"meshes"}},
            AllowedPath{".nif", {"meshes"}},
            AllowedPath{".png", {"textures"}},
            AllowedPath{".tga", {"textures"}},
            AllowedPath{".tri", {"meshes"}},
        };
        sets.textureFiles = {
            AllowedPath{".dds", {"textures"}},
        };
        sets.incompressibleFiles = {AllowedPath{".dds", {"interface"}},
                                    AllowedPath{".dlstrings", {"strings"}},
                                    AllowedPath{".fuz", {"sound"}},
                                    AllowedPath{".fxp", {"shadersfx"}},
                                    AllowedPath{".gid", {"grass"}},
                                    AllowedPath{".gfx", {"interface"}},
                                    AllowedPath{".hkc", {"meshes"}},
                                    AllowedPath{".hkt", {"meshes"}},
                                    AllowedPath{".ilstrings", {"strings"}},
                                    AllowedPath{".ini", {"meshes"}},
                                    AllowedPath{".lip", {"sound"}},
                                    AllowedPath{".lnk", {"grass"}},
                                    AllowedPath{".lod", {"lodsettings"}},
                                    AllowedPath{".ogg", {"sound"}},
                                    AllowedPath{".pex", {"scripts"}},
                                    AllowedPath{".psc", {"scripts"}},
                                    AllowedPath{".seq", {"seq"}},
                                    AllowedPath{".strings", {"strings"}},
                                    AllowedPath{".swf", {"interface"}},
                                    AllowedPath{".txt", {"interface", "meshes", "scripts"}},
                                    AllowedPath{".wav", {"sound"}},
                                    AllowedPath{".xml", {"dialogueviews"}},
                                    AllowedPath{".xwm", {"music", "sound", "music"}}};
        return sets;
    }();

    switch (game)
    {
        case Oblivion:
        {
            static GameSettings sets = [=] {
                GameSettings s     = defaultSets;
                s.game             = Games::Oblivion;
                s.format           = version::tes4;
                s.textureFormat    = std::nullopt;
                sets.textureSuffix = std::nullopt;
                s.pluginExtensions = {".esm", ".esp"};
                sets.sDummyPlugin  = std::vector(std::begin(Dummy::Oblivion), std::end(Dummy::Oblivion));
                return s;
            }();
            return sets;
        }
        case FNV:
        {
            static GameSettings sets = [=] {
                GameSettings s     = defaultSets;
                s.game             = Games::FNV;
                s.format           = version::tes4;
                s.textureFormat    = std::nullopt;
                sets.textureSuffix = std::nullopt;
                s.pluginExtensions = {".esm", ".esp"};
                sets.sDummyPlugin  = std::vector(std::begin(Dummy::FNV), std::end(Dummy::FNV));
                return s;
            }();
            return sets;
        }
        case SLE:
        {
            static GameSettings sets = [=] {
                GameSettings s     = defaultSets;
                s.game             = Games::SLE;
                s.format           = version::tes5;
                s.textureFormat    = std::nullopt;
                s.suffix           = path{};
                s.textureSuffix    = std::nullopt;
                s.pluginExtensions = {".esm", ".esp"};
                sets.sDummyPlugin  = std::vector(std::begin(Dummy::Skyrim), std::end(Dummy::Skyrim));
                return s;
            }();
            return sets;
        }
        case SSE: return defaultSets;
        case FO4:
        {
            static GameSettings sets = [=] {
                GameSettings s    = defaultSets;
                s.game            = Games::FO4;
                s.format          = version::fo4;
                s.textureFormat   = version::fo4dx;
                sets.extension    = ".ba2";
                s.suffix          = "Main";
                sets.sDummyPlugin = std::vector(std::begin(Dummy::Fallout4), std::end(Dummy::Fallout4));
                return s;
            }();
            return sets;
        }
        default: return defaultSets;
    }
}

inline bool AllowedPath::check(const fs::path &filepath, const fs::path &root) const
{
    auto const ext = filepath.extension().native();
    if (checkExtension && !str_compare<charT>(extension.native(), ext, false))
        return false;

    if (checkDirectory)
    {
        auto const &relative = filepath.lexically_relative(root);
        auto const dir       = [&relative] {
            if (relative.empty())
                return AllowedPath::root;
            else
                return to_lower(*relative.begin());
        }();
        return contains(directories, dir);
    }
    return true;
}

} // namespace BethUtil::BSA
