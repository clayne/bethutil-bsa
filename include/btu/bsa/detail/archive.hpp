#pragma once

#include <bsa/bsa.hpp>

#include <functional>

namespace libbsa = ::bsa;

namespace btu::bsa {
enum class version : std::uint32_t
{
    tes3  = 1,
    tes4  = libbsa::detail::to_underlying(libbsa::tes4::version::tes4),
    fo3   = libbsa::detail::to_underlying(libbsa::tes4::version::fo3),
    tes5  = libbsa::detail::to_underlying(libbsa::tes4::version::tes5),
    sse   = libbsa::detail::to_underlying(libbsa::tes4::version::sse),
    fo4   = libbsa::detail::to_underlying(libbsa::fo4::format::general),
    fo4dx = libbsa::detail::to_underlying(libbsa::fo4::format::directx),
};

using underlying_archive = std::variant<libbsa::tes3::archive, libbsa::tes4::archive, libbsa::fo4::archive>;

namespace detail {
template<class... Ts>
struct overload : Ts...
{
    using Ts::operator()...;
};
template<class... Ts>
overload(Ts...) -> overload<Ts...>;

[[nodiscard]] auto read_file(const std::filesystem::path &a_path) -> std::vector<std::byte>;

template<class... Keys>
[[nodiscard]] auto virtual_to_local_path(Keys &&...a_keys) -> std::string
{
    std::string local;
    ((local += a_keys.name(), local += '/'), ...);
    local.pop_back();

    for (auto &c : local)
    {
        if (c == '\\' || c == '/')
        {
            c = std::filesystem::path::preferred_separator;
        }
    }

    return local;
}

[[nodiscard]] auto get_archive_identifier(underlying_archive archive) -> std::string_view;

template<typename Version>
[[nodiscard]] auto archive_version(underlying_archive archive, version a_version) -> Version;
} // namespace detail

class archive
{
public:
    archive(const std::filesystem::path &a_path); // Read
    archive(version a_version, bool a_compressed);

    auto read(const std::filesystem::path &a_path) -> version;
    void write(std::filesystem::path a_path);

    void add_file(const std::filesystem::path &a_root, const std::filesystem::path &a_path);
    void add_file(const std::filesystem::path &a_relative, std::vector<std::byte> a_data);

    using iteration_callback = std::function<void(const std::filesystem::path &, std::span<const std::byte>)>;
    void iterate_files(const iteration_callback &a_callback, bool skip_compressed = false);

    version get_version() const noexcept;
    const underlying_archive &get_archive() const noexcept;

private:
    underlying_archive _archive;
    version _version;
    bool _compressed;
};

} // namespace btu::bsa
