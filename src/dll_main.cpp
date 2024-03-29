// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "macros.h"
#include "version.h"

import <std.h>;
import <win.h>;
import winrt;

import netpbm_bitmap_decoder;
import errors;
import guids;
import registry;
import util;

using std::array;
using std::format;
using std::wstring;
using namespace std::string_literals;

namespace {

constexpr wchar_t mime_types[]{L"image/x-portable-graymap,image/x-portable-pixmap"};
constexpr wchar_t file_extensions[]{L".pgm,.ppm"};

void register_general_decoder_settings(const GUID& class_id, const GUID& wic_category_id, const wchar_t* friendly_name,
                                       const std::span<const GUID*> formats)
{
    const wstring sub_key = LR"(SOFTWARE\Classes\CLSID\)" + guid_to_string(class_id);
    registry::set_value(sub_key, L"ArbitrationPriority", 10);
    registry::set_value(sub_key, L"Author", L"Victor Derks");
    registry::set_value(sub_key, L"ColorManagementVersion", L"1.0.0.0");
    registry::set_value(sub_key, L"ContainerFormat", guid_to_string(id::container_format_netpbm).c_str());
    registry::set_value(sub_key, L"Description", L"Netpbm Codec");
    registry::set_value(sub_key, L"FileExtensions", file_extensions);
    registry::set_value(sub_key, L"FriendlyName", friendly_name);
    registry::set_value(sub_key, L"MimeTypes", mime_types);
    registry::set_value(sub_key, L"SpecVersion", L"1.0.0.0");
    registry::set_value(sub_key, L"SupportAnimation", 0U);
    registry::set_value(sub_key, L"SupportChromaKey", 0U);
    registry::set_value(sub_key, L"SupportLossless", 1U);
    registry::set_value(sub_key, L"SupportMultiframe", 0U);
    registry::set_value(sub_key, L"Vendor", guid_to_string(id::vendor_victor_derks).c_str());
    registry::set_value(sub_key, L"Version", VERSION);

    const wstring formats_sub_key{sub_key + LR"(\Formats\)"};
    for (const GUID* format : formats)
    {
        registry::set_value(formats_sub_key + guid_to_string(*format), L"", L"");
    }

    // COM co-create registration.
    const wstring inproc_server_sub_key{sub_key + LR"(\InprocServer32\)"};
    registry::set_value(inproc_server_sub_key, L"", get_module_path().c_str());
    registry::set_value(inproc_server_sub_key, L"ThreadingModel", L"Both");

    // WIC category registration.
    const wstring category_id_key{LR"(SOFTWARE\Classes\CLSID\)" + guid_to_string(wic_category_id) + LR"(\Instance\)" +
                                  guid_to_string(id::netpbm_decoder)};
    registry::set_value(category_id_key, L"FriendlyName", friendly_name);
    registry::set_value(category_id_key, L"CLSID", guid_to_string(class_id).c_str());
}

void register_decoder_file_extension(const wchar_t* file_type_name, const wchar_t* file_extension, const wchar_t* mime_type)
{
    const wstring extension_sub_key{LR"(SOFTWARE\Classes\"s + file_extension + LR"\)"};
    registry::set_value(extension_sub_key, L"", file_type_name);
    registry::set_value(extension_sub_key, L"Content Type", mime_type);
    registry::set_value(extension_sub_key, L"PerceivedType", L"image");

    registry::set_value(LR"(SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\KindMap)", file_extension, L"picture");

    // Register with the Windows Thumbnail Cache
    // Use the Microsoft Windows provided PhotoThumbnailProvider module to generate the thumbnail
    constexpr wchar_t class_id_thumbnail_provider[]{L"{e357fccd-a995-4576-b01f-234630154e96}"};
    constexpr wchar_t class_id_photo_thumbnail_provider[]{L"{c7657c4a-9f68-40fa-a4df-96bc08eb3551}"};

    registry::set_value(format(LR"(SOFTWARE\Classes\{}\ShellEx\{})", file_type_name, class_id_thumbnail_provider), L"",
                        class_id_photo_thumbnail_provider);
    registry::set_value(
        format(LR"(SOFTWARE\Classes\SystemFileAssociations\{}\ShellEx\{})", file_extension, class_id_thumbnail_provider),
        L"", class_id_photo_thumbnail_provider);

    // Register with the legacy Windows Photo Viewer (still installed on Windows 11 and 10): just forward to the TIFF
    // registration.
    registry::set_value(LR"(SOFTWARE\Microsoft\Windows Photo Viewer\Capabilities\FileAssociations)", file_extension,
                        L"PhotoViewer.FileAssoc.Tiff");
}

 void register_decoder_pattern(const wstring& sub_key, const int index, const std::span<const std::byte> pattern)
{
     const wstring patterns_sub_key{sub_key + LR"(\Patterns\)" + std::to_wstring(index)};
     constexpr array mask{0xFF_byte, 0xFF_byte};
     registry::set_value(patterns_sub_key, L"Length", static_cast<uint32_t>(pattern.size()));
     registry::set_value(patterns_sub_key, L"Position", 0U);
     registry::set_value(patterns_sub_key, L"Mask", mask);
     registry::set_value(patterns_sub_key, L"Pattern", pattern);
 }

void register_decoder()
{
    array formats{&GUID_WICPixelFormat2bppGray, &GUID_WICPixelFormat4bppGray, &GUID_WICPixelFormat8bppGray,
                  &GUID_WICPixelFormat16bppGray, &GUID_WICPixelFormat24bppRGB};
    register_general_decoder_settings(id::netpbm_decoder, CATID_WICBitmapDecoders, L"Netpbm Decoder", formats);

    const wstring sub_key{LR"(SOFTWARE\Classes\CLSID\)" + guid_to_string(id::netpbm_decoder)};

    // Register the byte pattern that allows WICs to identify files as our image type.
    register_decoder_pattern(sub_key, 0, array{std::byte{0x50}, std::byte{0x35}});
    register_decoder_pattern(sub_key, 0, array{std::byte{0x50}, std::byte{0x36}});

    register_decoder_file_extension(L"pgmfile", L".pgm", L"image/x-portable-graymap");
    register_decoder_file_extension(L"ppmfile", L".ppm", L"image/x-portable-pixmap");
}

[[nodiscard]] HRESULT unregister(const GUID& class_id, const GUID& wic_category_id)
{
    const wstring sub_key{LR"(SOFTWARE\Classes\CLSID\)" + guid_to_string(class_id)};
    const HRESULT result1{registry::delete_tree(sub_key.c_str())};

    const wstring category_id_key{LR"(SOFTWARE\Classes\CLSID\)" + guid_to_string(wic_category_id) + LR"(\Instance\)" +
                                  guid_to_string(class_id)};
    const HRESULT result2{registry::delete_tree(category_id_key.c_str())};

    return failed(result1) ? result1 : result2;
}

} // namespace

// ReSharper disable CppInconsistentNaming
// ReSharper disable CppParameterNamesMismatch

BOOL APIENTRY DllMain(const HMODULE module, const DWORD reason_for_call, void* /*reserved*/) noexcept
{
    switch (reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        TRACE("netpbm-wic-codec::DllMain DLL_PROCESS_ATTACH \n");
        VERIFY(DisableThreadLibraryCalls(module));
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        TRACE("netpbm-wic-codec::DllMain DLL_PROCESS_DETACH \n");
        break;

    default:
        TRACE("netpbm-wic-codec::DllMain bad reason_for call\n");
        return false;
    }

    return true;
}

_Check_return_ HRESULT __stdcall DllGetClassObject(_In_ GUID const& class_id, _In_ GUID const& interface_id,
                                                   _Outptr_ void** result)
try
{
    if (class_id != id::netpbm_decoder)
        return error_class_not_available;

    create_netpbm_bitmap_decoder_factory(interface_id, result);
    return error_ok;
}
catch (...)
{
    return winrt::to_hresult();
}

// Purpose: Used to determine whether the COM sub-system can unload the DLL from memory.
extern "C" __control_entrypoint(DllExport) HRESULT __stdcall DllCanUnloadNow()
{
    const auto result{winrt::get_module_lock() ? S_FALSE : S_OK};
    TRACE("netpbm-wic-codec::DllCanUnloadNow hr = {} (0 = S_OK -> unload OK)\n", result);
    return result;
}

HRESULT __stdcall DllRegisterServer()
try
{
    register_decoder();

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);

    return error_ok;
}
catch (...)
{
    TRACE("netpbm-wic-codec::DllRegisterServer failed hr = {}\n", winrt::to_hresult());
    return self_registration::error_class;
}

HRESULT __stdcall DllUnregisterServer()
try
{
    // Note: keep the .pgm file registration intact.
    return unregister(id::netpbm_decoder, CATID_WICBitmapDecoders);
}
catch (...)
{
    const HRESULT hresult{winrt::to_hresult()};
    TRACE("netpbm-wic-codec::DllUnregisterServer failed hr = {}\n", hresult);
    return hresult;
}

// ReSharper restore CppParameterNamesMismatch
// ReSharper restore CppInconsistentNaming
