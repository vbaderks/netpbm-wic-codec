// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT
module;

#include "pch.h"

export module guids;


export
{
    // {06891bbe-cc02-4bb2-9cf0-303fc4e668c3}
    inline constexpr GUID CLSID_NetPbmDecoder{0x6891bbe, 0xcc02, 0x4bb2, {0x9c, 0xf0, 0x30, 0x3f, 0xc4, 0xe6, 0x68, 0xc3}};

    // {70ab66f5-cd48-43a1-aa29-10131b7f4ff1}
    inline constexpr GUID GUID_ContainerFormatNetPbm{
        0x70ab66f5, 0xcd48, 0x43a1, {0xaa, 0x29, 0x10, 0x13, 0x1b, 0x7f, 0x4f, 0xf1}};

    // {87a8e9f4-8aac-4667-bcfd-56535c80a269}
    inline constexpr GUID GUID_VendorVictorDerks{
        0x87a8e9f4, 0x8aac, 0x4667, {0xbc, 0xfd, 0x56, 0x53, 0x5c, 0x80, 0xa2, 0x69}};
}
