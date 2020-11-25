// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#include "pch.h"

#include <winerror.h>
#include <winrt/base.h>

#include "class_factory.h"

export module netpbm_bitmap_decoder;

#ifdef NDEBUG

#define TRACE __noop

#else

template<typename... Args>
void trace(char const* const message, Args... args) noexcept
{
    char buffer[1024];
    static_cast<void>(snprintf(buffer, sizeof buffer, message, args...)); // NOLINT(cppcoreguidelines-pro-type-vararg)
    OutputDebugStringA(buffer);
}

#define TRACE trace

#endif


using winrt::check_hresult;
using winrt::com_ptr;
using winrt::implements;
using winrt::make;
using winrt::to_hresult;


struct netpbm_bitmap_decoder final : implements<netpbm_bitmap_decoder, IWICBitmapDecoder>
{
    // IWICBitmapDecoder
    HRESULT __stdcall QueryCapability(_In_ IStream* stream, _Out_ DWORD* capability) noexcept override
    {
        TRACE("%p jpegls_bitmap_decoder::QueryCapability.1, stream=%p, capability=%p\n", this, stream, capability);
        return error_fail;
    }

    HRESULT __stdcall Initialize(_In_ IStream* stream, [[maybe_unused]] const WICDecodeOptions cache_options) noexcept override
    {
        TRACE("%p jpegls_bitmap_decoder::Initialize, stream=%p, cache_options=%d\n", this, stream, cache_options);
        return error_fail;
    }

    HRESULT __stdcall GetContainerFormat(_Out_ GUID* container_format) noexcept override
    {
        TRACE("%p jpegls_bitmap_decoder::GetContainerFormat, container_format=%p\n", this, container_format);
        return error_fail;
    }

    HRESULT __stdcall GetDecoderInfo(_Outptr_ IWICBitmapDecoderInfo** decoder_info) noexcept override
    {
        TRACE("%p jpegls_bitmap_decoder::GetContainerFormat, decoder_info=%p\n", this, decoder_info);
        return error_fail;
    }

    HRESULT __stdcall CopyPalette([[maybe_unused]] _In_ IWICPalette* palette) noexcept override
    {
        TRACE("%p jpegls_bitmap_decoder::CopyPalette, palette=%p\n", this, palette);
        return error_fail;
    }

    HRESULT __stdcall GetMetadataQueryReader([[maybe_unused]] _Outptr_ IWICMetadataQueryReader** metadata_query_reader) noexcept override
    {
        TRACE("%p jpegls_bitmap_decoder::GetMetadataQueryReader, metadata_query_reader=%p\n", this, metadata_query_reader);
        return error_fail;
    }

    HRESULT __stdcall GetPreview([[maybe_unused]] _Outptr_ IWICBitmapSource** bitmap_source) noexcept override
    {
        TRACE("%p jpegls_bitmap_decoder::GetPreview, bitmap_source=%p\n", this, bitmap_source);
        return error_fail;
    }

    HRESULT __stdcall GetColorContexts([[maybe_unused]] const uint32_t count, [[maybe_unused]] IWICColorContext** color_contexts, [[maybe_unused]] uint32_t* actual_count) noexcept override
    {
        TRACE("%p jpegls_bitmap_decoder::GetColorContexts, count=%u, color_contexts=%p, actual_count=%p\n", this, count, color_contexts, actual_count);
        return error_fail;
    }

    HRESULT __stdcall GetThumbnail([[maybe_unused]] _Outptr_ IWICBitmapSource** thumbnail) noexcept override
    {
        TRACE("%p jpegls_bitmap_decoder::GetThumbnail, thumbnail=%p\n", this, thumbnail);
        return error_fail;
    }

    HRESULT __stdcall GetFrameCount(_Out_ uint32_t* count) noexcept override
    {
        TRACE("%p jpegls_bitmap_decoder::GetFrameCount, count=%p\n", this, count);
        return error_fail;
    }

    HRESULT __stdcall GetFrame(const uint32_t index, _Outptr_ IWICBitmapFrameDecode** bitmap_frame_decode) noexcept override
    {
        TRACE("%p jpegls_bitmap_decoder::GetFrame, index=%d, bitmap_frame_decode=%p\n", this, index, bitmap_frame_decode);
        return error_fail;
    }

private:
    IWICImagingFactory* imaging_factory()
    {
        if (!imaging_factory_)
        {
            check_hresult(CoCreateInstance(CLSID_WICImagingFactory,
                                           nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(imaging_factory_.put())));
        }

        return imaging_factory_.get();
    }

    com_ptr<IWICImagingFactory> imaging_factory_;
    com_ptr<IStream> stream_;
    com_ptr<IWICBitmapFrameDecode> bitmap_frame_decode_;
};

export HRESULT create_netpbm_bitmap_decoder_factory(_In_ GUID const& interface_id, _Outptr_ void** result)
{
    return make<class_factory<netpbm_bitmap_decoder>>()->QueryInterface(interface_id, result);
}
