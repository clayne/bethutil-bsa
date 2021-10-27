/* Copyright (C) 2021 Edgar B
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "DDS.h"
#include "btu/bsa/detail/backends/archive.hpp"

#include <DirectXTex.h>
#include <bsa/fo4.hpp>

namespace btu::bsa::detail {
namespace libbsa = ::bsa;

// Copied from https://github.com/microsoft/DirectXTex/blob/master/DirectXTex/DirectXTexDDS.cpp#L558
// Required as the function is private
_Use_decl_annotations_ auto encode_dds_header(const DirectX::TexMetadata &metadata,
                                              DirectX::DDS_FLAGS flags,
                                              void *p_destination,
                                              size_t maxsize,
                                              size_t &required) noexcept -> HRESULT
{
    using namespace DirectX;
    if (!IsValid(metadata.format))
        return E_INVALIDARG;

    if (IsPalettized(metadata.format))
        return -1;

    if (metadata.arraySize > 1)
    {
        if ((metadata.arraySize != 6) || (metadata.dimension != TEX_DIMENSION_TEXTURE2D)
            || !(metadata.IsCubemap()))
        {
            // Texture1D arrays, Texture2D arrays, and Cubemap arrays must be stored using 'DX10' extended header
            if (flags & DDS_FLAGS_FORCE_DX9_LEGACY)
                return -2;

            flags |= DDS_FLAGS_FORCE_DX10_EXT;
        }
    }

    if (flags & DDS_FLAGS_FORCE_DX10_EXT_MISC2)
    {
        flags |= DDS_FLAGS_FORCE_DX10_EXT;
    }

    DDS_PIXELFORMAT ddpf = {};
    if (!(flags & DDS_FLAGS_FORCE_DX10_EXT))
    {
        switch (metadata.format)
        {
            case DXGI_FORMAT_R8G8B8A8_UNORM: memcpy(&ddpf, &DDSPF_A8B8G8R8, sizeof(DDS_PIXELFORMAT)); break;
            case DXGI_FORMAT_R16G16_UNORM: memcpy(&ddpf, &DDSPF_G16R16, sizeof(DDS_PIXELFORMAT)); break;
            case DXGI_FORMAT_R8G8_UNORM: memcpy(&ddpf, &DDSPF_A8L8, sizeof(DDS_PIXELFORMAT)); break;
            case DXGI_FORMAT_R16_UNORM: memcpy(&ddpf, &DDSPF_L16, sizeof(DDS_PIXELFORMAT)); break;
            case DXGI_FORMAT_R8_UNORM: memcpy(&ddpf, &DDSPF_L8, sizeof(DDS_PIXELFORMAT)); break;
            case DXGI_FORMAT_A8_UNORM: memcpy(&ddpf, &DDSPF_A8, sizeof(DDS_PIXELFORMAT)); break;
            case DXGI_FORMAT_R8G8_B8G8_UNORM: memcpy(&ddpf, &DDSPF_R8G8_B8G8, sizeof(DDS_PIXELFORMAT)); break;
            case DXGI_FORMAT_G8R8_G8B8_UNORM: memcpy(&ddpf, &DDSPF_G8R8_G8B8, sizeof(DDS_PIXELFORMAT)); break;
            case DXGI_FORMAT_BC1_UNORM: memcpy(&ddpf, &DDSPF_DXT1, sizeof(DDS_PIXELFORMAT)); break;
            case DXGI_FORMAT_BC2_UNORM:
                memcpy(&ddpf, metadata.IsPMAlpha() ? (&DDSPF_DXT2) : (&DDSPF_DXT3), sizeof(DDS_PIXELFORMAT));
                break;
            case DXGI_FORMAT_BC3_UNORM:
                memcpy(&ddpf, metadata.IsPMAlpha() ? (&DDSPF_DXT4) : (&DDSPF_DXT5), sizeof(DDS_PIXELFORMAT));
                break;
            case DXGI_FORMAT_BC4_SNORM: memcpy(&ddpf, &DDSPF_BC4_SNORM, sizeof(DDS_PIXELFORMAT)); break;
            case DXGI_FORMAT_BC5_SNORM: memcpy(&ddpf, &DDSPF_BC5_SNORM, sizeof(DDS_PIXELFORMAT)); break;
            case DXGI_FORMAT_B5G6R5_UNORM: memcpy(&ddpf, &DDSPF_R5G6B5, sizeof(DDS_PIXELFORMAT)); break;
            case DXGI_FORMAT_B5G5R5A1_UNORM: memcpy(&ddpf, &DDSPF_A1R5G5B5, sizeof(DDS_PIXELFORMAT)); break;
            case DXGI_FORMAT_R8G8_SNORM: memcpy(&ddpf, &DDSPF_V8U8, sizeof(DDS_PIXELFORMAT)); break;
            case DXGI_FORMAT_R8G8B8A8_SNORM: memcpy(&ddpf, &DDSPF_Q8W8V8U8, sizeof(DDS_PIXELFORMAT)); break;
            case DXGI_FORMAT_R16G16_SNORM: memcpy(&ddpf, &DDSPF_V16U16, sizeof(DDS_PIXELFORMAT)); break;
            case DXGI_FORMAT_B8G8R8A8_UNORM:
                memcpy(&ddpf, &DDSPF_A8R8G8B8, sizeof(DDS_PIXELFORMAT));
                break; // DXGI 1.1
            case DXGI_FORMAT_B8G8R8X8_UNORM:
                memcpy(&ddpf, &DDSPF_X8R8G8B8, sizeof(DDS_PIXELFORMAT));
                break; // DXGI 1.1
            case DXGI_FORMAT_B4G4R4A4_UNORM:
                memcpy(&ddpf, &DDSPF_A4R4G4B4, sizeof(DDS_PIXELFORMAT));
                break; // DXGI 1.2
            case DXGI_FORMAT_YUY2:
                memcpy(&ddpf, &DDSPF_YUY2, sizeof(DDS_PIXELFORMAT));
                break; // DXGI 1.2

            // Legacy D3DX formats using D3DFMT enum value as FourCC
            case DXGI_FORMAT_R32G32B32A32_FLOAT:
                ddpf.size   = sizeof(DDS_PIXELFORMAT);
                ddpf.flags  = DDS_FOURCC;
                ddpf.fourCC = 116; // D3DFMT_A32B32G32R32F
                break;
            case DXGI_FORMAT_R16G16B16A16_FLOAT:
                ddpf.size   = sizeof(DDS_PIXELFORMAT);
                ddpf.flags  = DDS_FOURCC;
                ddpf.fourCC = 113; // D3DFMT_A16B16G16R16F
                break;
            case DXGI_FORMAT_R16G16B16A16_UNORM:
                ddpf.size   = sizeof(DDS_PIXELFORMAT);
                ddpf.flags  = DDS_FOURCC;
                ddpf.fourCC = 36; // D3DFMT_A16B16G16R16
                break;
            case DXGI_FORMAT_R16G16B16A16_SNORM:
                ddpf.size   = sizeof(DDS_PIXELFORMAT);
                ddpf.flags  = DDS_FOURCC;
                ddpf.fourCC = 110; // D3DFMT_Q16W16V16U16
                break;
            case DXGI_FORMAT_R32G32_FLOAT:
                ddpf.size   = sizeof(DDS_PIXELFORMAT);
                ddpf.flags  = DDS_FOURCC;
                ddpf.fourCC = 115; // D3DFMT_G32R32F
                break;
            case DXGI_FORMAT_R16G16_FLOAT:
                ddpf.size   = sizeof(DDS_PIXELFORMAT);
                ddpf.flags  = DDS_FOURCC;
                ddpf.fourCC = 112; // D3DFMT_G16R16F
                break;
            case DXGI_FORMAT_R32_FLOAT:
                ddpf.size   = sizeof(DDS_PIXELFORMAT);
                ddpf.flags  = DDS_FOURCC;
                ddpf.fourCC = 114; // D3DFMT_R32F
                break;
            case DXGI_FORMAT_R16_FLOAT:
                ddpf.size   = sizeof(DDS_PIXELFORMAT);
                ddpf.flags  = DDS_FOURCC;
                ddpf.fourCC = 111; // D3DFMT_R16F
                break;

            // DX9 legacy pixel formats
            case DXGI_FORMAT_R10G10B10A2_UNORM:
                if (flags & DDS_FLAGS_FORCE_DX9_LEGACY)
                {
                    constexpr DDS_PIXELFORMAT DDSPF_A2B10G10R10 = {sizeof(DDS_PIXELFORMAT),
                                                                   DDS_RGBA,
                                                                   0,
                                                                   32,
                                                                   0x3ff00000,
                                                                   0x000ffc00,
                                                                   0x000003ff,
                                                                   0xc0000000};
                    // Write using the 'incorrect' mask version to match D3DX bug
                    memcpy(&ddpf, &DDSPF_A2B10G10R10, sizeof(DDS_PIXELFORMAT));
                }
                break;

            case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
                if (flags & DDS_FLAGS_FORCE_DX9_LEGACY)
                {
                    memcpy(&ddpf, &DDSPF_A8B8G8R8, sizeof(DDS_PIXELFORMAT));
                }
                break;

            case DXGI_FORMAT_BC1_UNORM_SRGB:
                if (flags & DDS_FLAGS_FORCE_DX9_LEGACY)
                {
                    memcpy(&ddpf, &DDSPF_DXT1, sizeof(DDS_PIXELFORMAT));
                }
                break;

            case DXGI_FORMAT_BC2_UNORM_SRGB:
                if (flags & DDS_FLAGS_FORCE_DX9_LEGACY)
                {
                    memcpy(&ddpf,
                           metadata.IsPMAlpha() ? (&DDSPF_DXT2) : (&DDSPF_DXT3),
                           sizeof(DDS_PIXELFORMAT));
                }
                break;

            case DXGI_FORMAT_BC3_UNORM_SRGB:
                if (flags & DDS_FLAGS_FORCE_DX9_LEGACY)
                {
                    memcpy(&ddpf,
                           metadata.IsPMAlpha() ? (&DDSPF_DXT4) : (&DDSPF_DXT5),
                           sizeof(DDS_PIXELFORMAT));
                }
                break;

            case DXGI_FORMAT_BC4_UNORM:
                memcpy(&ddpf, &DDSPF_BC4_UNORM, sizeof(DDS_PIXELFORMAT));
                if (flags & DDS_FLAGS_FORCE_DX9_LEGACY)
                {
                    ddpf.fourCC = MAKEFOURCC('A', 'T', 'I', '1');
                }
                break;

            case DXGI_FORMAT_BC5_UNORM:
                memcpy(&ddpf, &DDSPF_BC5_UNORM, sizeof(DDS_PIXELFORMAT));
                if (flags & DDS_FLAGS_FORCE_DX9_LEGACY)
                {
                    ddpf.fourCC = MAKEFOURCC('A', 'T', 'I', '2');
                }
                break;

            case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
                if (flags & DDS_FLAGS_FORCE_DX9_LEGACY)
                {
                    memcpy(&ddpf, &DDSPF_A8R8G8B8, sizeof(DDS_PIXELFORMAT));
                }
                break;

            case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
                if (flags & DDS_FLAGS_FORCE_DX9_LEGACY)
                {
                    memcpy(&ddpf, &DDSPF_X8R8G8B8, sizeof(DDS_PIXELFORMAT));
                }
                break;

            default: break;
        }
    }

    required = sizeof(uint32_t) + sizeof(DDS_HEADER);

    if (ddpf.size == 0)
    {
        if (flags & DDS_FLAGS_FORCE_DX9_LEGACY)
            return -3;

        required += sizeof(DDS_HEADER_DXT10);
    }

    if (!p_destination)
        return S_OK;

    if (maxsize < required)
        return E_NOT_SUFFICIENT_BUFFER;

    *static_cast<uint32_t *>(p_destination) = DDS_MAGIC;

    auto header = reinterpret_cast<DDS_HEADER *>(static_cast<uint8_t *>(p_destination) + sizeof(uint32_t));
    assert(header);

    memset(header, 0, sizeof(DDS_HEADER));
    header->size  = sizeof(DDS_HEADER);
    header->flags = DDS_HEADER_FLAGS_TEXTURE;
    header->caps  = DDS_SURFACE_FLAGS_TEXTURE;

    if (metadata.mipLevels > 0)
    {
        header->flags |= DDS_HEADER_FLAGS_MIPMAP;

        if (metadata.mipLevels > UINT16_MAX)
            return E_INVALIDARG;

        header->mipMapCount = static_cast<uint32_t>(metadata.mipLevels);

        if (header->mipMapCount > 1)
            header->caps |= DDS_SURFACE_FLAGS_MIPMAP;
    }

    switch (metadata.dimension)
    {
        case TEX_DIMENSION_TEXTURE1D:
            if (metadata.width > UINT32_MAX)
                return E_INVALIDARG;

            header->width  = static_cast<uint32_t>(metadata.width);
            header->height = header->depth = 1;
            break;

        case TEX_DIMENSION_TEXTURE2D:
            if (metadata.height > UINT32_MAX || metadata.width > UINT32_MAX)
                return E_INVALIDARG;

            header->height = static_cast<uint32_t>(metadata.height);
            header->width  = static_cast<uint32_t>(metadata.width);
            header->depth  = 1;

            if (metadata.IsCubemap())
            {
                header->caps |= DDS_SURFACE_FLAGS_CUBEMAP;
                header->caps2 |= DDS_CUBEMAP_ALLFACES;
            }
            break;

        case TEX_DIMENSION_TEXTURE3D:
            if (metadata.height > UINT32_MAX || metadata.width > UINT32_MAX || metadata.depth > UINT16_MAX)
                return E_INVALIDARG;

            header->flags |= DDS_HEADER_FLAGS_VOLUME;
            header->caps2 |= DDS_FLAGS_VOLUME;
            header->height = static_cast<uint32_t>(metadata.height);
            header->width  = static_cast<uint32_t>(metadata.width);
            header->depth  = static_cast<uint32_t>(metadata.depth);
            break;

        default: return E_FAIL;
    }

    size_t rowPitch, slicePitch;
    HRESULT hr
        = ComputePitch(metadata.format, metadata.width, metadata.height, rowPitch, slicePitch, CP_FLAGS_NONE);
    if (FAILED(hr))
        return hr;

    if (slicePitch > UINT32_MAX || rowPitch > UINT32_MAX)
        return E_FAIL;

    if (IsCompressed(metadata.format))
    {
        header->flags |= DDS_HEADER_FLAGS_LINEARSIZE;
        header->pitchOrLinearSize = static_cast<uint32_t>(slicePitch);
    }
    else
    {
        header->flags |= DDS_HEADER_FLAGS_PITCH;
        header->pitchOrLinearSize = static_cast<uint32_t>(rowPitch);
    }

    if (ddpf.size == 0)
    {
        memcpy(&header->ddspf, &DDSPF_DX10, sizeof(DDS_PIXELFORMAT));

        auto ext = reinterpret_cast<DDS_HEADER_DXT10 *>(reinterpret_cast<uint8_t *>(header)
                                                        + sizeof(DDS_HEADER));
        assert(ext);

        memset(ext, 0, sizeof(DDS_HEADER_DXT10));
        ext->dxgiFormat        = metadata.format;
        ext->resourceDimension = metadata.dimension;

        if (metadata.arraySize > UINT16_MAX)
            return E_INVALIDARG;

        static_assert(static_cast<int>(TEX_MISC_TEXTURECUBE)
                          == static_cast<int>(DDS_RESOURCE_MISC_TEXTURECUBE),
                      "DDS header mismatch");

        ext->miscFlag = metadata.miscFlags & ~static_cast<uint32_t>(TEX_MISC_TEXTURECUBE);

        if (metadata.miscFlags & TEX_MISC_TEXTURECUBE)
        {
            ext->miscFlag |= TEX_MISC_TEXTURECUBE;
            assert((metadata.arraySize % 6) == 0);
            ext->arraySize = static_cast<UINT>(metadata.arraySize / 6);
        }
        else
        {
            ext->arraySize = static_cast<UINT>(metadata.arraySize);
        }

        static_assert(static_cast<int>(TEX_MISC2_ALPHA_MODE_MASK)
                          == static_cast<int>(DDS_MISC_FLAGS2_ALPHA_MODE_MASK),
                      "DDS header mismatch");

        static_assert(static_cast<int>(TEX_ALPHA_MODE_UNKNOWN) == static_cast<int>(DDS_ALPHA_MODE_UNKNOWN),
                      "DDS header mismatch");
        static_assert(static_cast<int>(TEX_ALPHA_MODE_STRAIGHT) == static_cast<int>(DDS_ALPHA_MODE_STRAIGHT),
                      "DDS header mismatch");
        static_assert(static_cast<int>(TEX_ALPHA_MODE_PREMULTIPLIED)
                          == static_cast<int>(DDS_ALPHA_MODE_PREMULTIPLIED),
                      "DDS header mismatch");
        static_assert(static_cast<int>(TEX_ALPHA_MODE_OPAQUE) == static_cast<int>(DDS_ALPHA_MODE_OPAQUE),
                      "DDS header mismatch");
        static_assert(static_cast<int>(TEX_ALPHA_MODE_CUSTOM) == static_cast<int>(DDS_ALPHA_MODE_CUSTOM),
                      "DDS header mismatch");

        if (flags & DDS_FLAGS_FORCE_DX10_EXT_MISC2)
        {
            // This was formerly 'reserved'. D3DX10 and D3DX11 will fail if this value is anything other than 0
            ext->miscFlags2 = metadata.miscFlags2;
        }
    }
    else
    {
        memcpy(&header->ddspf, &ddpf, sizeof(ddpf));
    }

    return S_OK;
}
template<size_t N, typename It, typename T = typename std::iterator_traits<It>::value_type>
auto split(It start, It end) -> std::array<std::vector<T>, N>
{
    std::array<std::vector<T>, N> rtn;
    const size_t count = std::distance(start, end);

    size_t out_idx = 0;
    size_t limit   = std::ceil(static_cast<double>(count) / N);
    for (size_t i = 0; i < count; i += limit)
    {
        rtn[out_idx] = std::vector(start + i, start + std::min<size_t>(i + limit, count)); // NOLINT
        out_idx++;
    }

    return rtn;
}

auto pack_fo4dx_file(std::span<std::byte> data, bool compress) -> libbsa::fo4::file
{
    DirectX::ScratchImage image;
    DirectX::TexMetadata info{};

    const auto hr = DirectX::LoadFromDDSMemory(data.data(), data.size(), DirectX::DDS_FLAGS_NONE, &info, image);

    if (FAILED(hr))
        throw libbsa::exception("Failed to read file as DDS");

    libbsa::fo4::file file;
    file.header.height    = info.height;
    file.header.width     = info.width;
    file.header.flags     = static_cast<std::uint8_t>(info.miscFlags);
    file.header.mip_count = std::max<size_t>(1, info.mipLevels);
    file.header.format    = static_cast<std::uint8_t>(info.format);

    auto img_chunks = split<4>(image.GetImages(), image.GetImages() + image.GetImageCount());

    size_t cur_mip = 0;
    for (const auto &img_chunk : img_chunks)
    {
        if (img_chunk.empty())
            continue;

        std::vector<std::byte> vec;

        for (const auto &img : img_chunk)
        {
            auto *const pix = reinterpret_cast<std::byte *>(img.pixels);
            vec.insert(vec.end(), pix, pix + img.slicePitch);
        }

        libbsa::fo4::chunk chunk;

        chunk.mips.first = cur_mip;
        cur_mip += img_chunk.size();
        chunk.mips.last = cur_mip;

        chunk.set_data(std::move(vec));
        if (compress)
            chunk.compress();

        file.emplace_back(std::move(chunk));
    }
    return file;
}

auto unpack_fo4dx_file(libbsa::fo4::file &file) -> std::vector<std::byte>
{
    const DirectX::TexMetadata info{
        .width     = file.header.width,
        .height    = file.header.height,
        .depth     = 1,
        .arraySize = 1,
        .mipLevels = file.header.mip_count,
        .miscFlags = file.header.flags,
        .format    = static_cast<DXGI_FORMAT>(file.header.format),
        .dimension = DirectX::TEX_DIMENSION_TEXTURE2D,
    };

    DirectX::Blob header;
    // Magic + both headers
    header.Initialize(sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER) + sizeof(DirectX::DDS_HEADER_DXT10));
    size_t required = 0;
    const auto hr   = encode_dds_header(info,
                                      DirectX::DDS_FLAGS_NONE,
                                      header.GetBufferPointer(),
                                      header.GetBufferSize(),
                                      required);
    if (FAILED(hr))
        throw libbsa::exception("DDS header encoding failed");

    auto *buf = static_cast<std::byte *>(header.GetBufferPointer());
    auto res  = std::vector(buf, buf + required);
    res.reserve(2'000'000); // 2mb; enough for most dds

    for (auto &chunk : file)
    {
        if (chunk.compressed())
            chunk.decompress();

        const auto bytes = chunk.as_bytes();
        res.insert(res.end(), bytes.begin(), bytes.end());
    }

    return res;
}
} // namespace btu::bsa::detail
