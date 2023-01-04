// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#include "winrt.h"

export module errors;

import <olectl.h>;

export {

inline constexpr HRESULT error_ok{S_OK};
inline constexpr HRESULT error_fail{E_FAIL};
inline constexpr HRESULT error_pointer{E_POINTER};
inline constexpr HRESULT error_no_aggregation{CLASS_E_NOAGGREGATION};
inline constexpr HRESULT error_class_not_available{CLASS_E_CLASSNOTAVAILABLE};
inline constexpr HRESULT error_invalid_argument{E_INVALIDARG};

namespace self_registration {

inline constexpr HRESULT error_class{SELFREG_E_CLASS};

}

namespace wincodec {

inline constexpr HRESULT error_palette_unavailable{WINCODEC_ERR_PALETTEUNAVAILABLE};
inline constexpr HRESULT error_unsupported_operation{WINCODEC_ERR_UNSUPPORTEDOPERATION};
inline constexpr HRESULT error_codec_no_thumbnail{WINCODEC_ERR_CODECNOTHUMBNAIL};
inline constexpr HRESULT error_frame_missing{WINCODEC_ERR_FRAMEMISSING};
inline constexpr HRESULT error_not_initialized{WINCODEC_ERR_NOTINITIALIZED};
inline constexpr HRESULT error_wrong_state{WINCODEC_ERR_WRONGSTATE};
inline constexpr HRESULT error_unsupported_pixel_format{WINCODEC_ERR_UNSUPPORTEDPIXELFORMAT};
inline constexpr HRESULT error_codec_too_many_scan_lines{WINCODEC_ERR_CODECTOOMANYSCANLINES};
inline constexpr HRESULT error_component_not_found{WINCODEC_ERR_COMPONENTNOTFOUND};
inline constexpr HRESULT error_bad_header{WINCODEC_ERR_BADHEADER};
inline constexpr HRESULT error_bad_image{WINCODEC_ERR_BADIMAGE};
inline constexpr HRESULT error_stream_not_available{WINCODEC_ERR_STREAMNOTAVAILABLE};
inline constexpr HRESULT error_stream_read{WINCODEC_ERR_STREAMREAD};

} // namespace wincodec

inline void check_hresult(const winrt::hresult result, const winrt::hresult result_to_throw)
{
    if (result < 0)
        throw_hresult(result_to_throw);
}

[[nodiscard]] constexpr bool failed(const winrt::hresult result) noexcept
{
    return result < 0;
}

template<typename T>
T* check_in_pointer(_In_ T* pointer)
{
    if (!pointer)
        winrt::throw_hresult(error_invalid_argument);

    return pointer;
}

template<typename T>
T* check_out_pointer(T* pointer)
{
    if (!pointer)
        winrt::throw_hresult(error_pointer);

    return pointer;
}

inline void check_condition(const bool condition, const winrt::hresult result_to_throw)
{
    if (!condition)
        throw_hresult(result_to_throw);
}

}
