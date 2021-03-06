﻿// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "pch.h"

#include "factory.h"
#include "portable_anymap_file.h"
#include "util.h"

#include <CppUnitTest.h>

import errors;

#include <array>
#include <span>
#include <vector>

using std::array;
using std::byte;
using std::span;
using std::vector;
using winrt::check_hresult;
using winrt::com_ptr;
using winrt::hresult;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


TEST_CLASS(netpbm_bitmap_frame_decode_test)
{
public:
    TEST_METHOD(GetSize) // NOLINT
    {
        com_ptr<IWICBitmapFrameDecode> bitmap_frame_decoder = create_frame_decoder(L"lena8b.pgm");

        uint32_t width;
        uint32_t height;

        const hresult result = bitmap_frame_decoder->GetSize(&width, &height);
        Assert::AreEqual(error_ok, result);
        Assert::AreEqual(512U, width);
        Assert::AreEqual(512U, height);
    }

    TEST_METHOD(CopyPalette) // NOLINT
    {
        com_ptr<IWICBitmapFrameDecode> bitmap_frame_decoder = create_frame_decoder(L"lena8b.pgm");

        com_ptr<IWICPalette> palette;
        check_hresult(imaging_factory()->CreatePalette(palette.put()));

        const hresult result = bitmap_frame_decoder->CopyPalette(palette.get());
        Assert::AreEqual(wincodec::error_palette_unavailable, result);
    }

    TEST_METHOD(GetThumbnail) // NOLINT
    {
        com_ptr<IWICBitmapFrameDecode> bitmap_frame_decoder = create_frame_decoder(L"lena8b.pgm");

        com_ptr<IWICBitmapSource> thumbnail;
        const hresult result = bitmap_frame_decoder->GetThumbnail(thumbnail.put());
        Assert::AreEqual(wincodec::error_codec_no_thumbnail, result);
    }

    TEST_METHOD(GetPixelFormat_8bit_image) // NOLINT
    {
        com_ptr<IWICBitmapFrameDecode> bitmap_frame_decoder = create_frame_decoder(L"lena8b.pgm");

        GUID pixel_format;
        const hresult result{bitmap_frame_decoder->GetPixelFormat(&pixel_format)};
        Assert::AreEqual(error_ok, result);
        Assert::IsTrue(GUID_WICPixelFormat8bppGray == pixel_format);
    }

    TEST_METHOD(GetPixelFormat_12bit_image) // NOLINT
    {
        com_ptr<IWICBitmapFrameDecode> bitmap_frame_decoder = create_frame_decoder(L"medical-m612-12bit.pgm");

        GUID pixel_format;
        const hresult result{bitmap_frame_decoder->GetPixelFormat(&pixel_format)};
        Assert::AreEqual(error_ok, result);
        Assert::IsTrue(GUID_WICPixelFormat16bppGray == pixel_format);
    }

    TEST_METHOD(GetPixelFormat_with_nullptr) // NOLINT
    {
        com_ptr<IWICBitmapFrameDecode> bitmap_frame_decoder = create_frame_decoder(L"lena8b.pgm");

        SUPPRESS_WARNING_6387_INVALID_ARGUMENT_NEXT_LINE
        const hresult result{bitmap_frame_decoder->GetPixelFormat(nullptr)};
        Assert::AreEqual(error_invalid_argument, result);
    }

    TEST_METHOD(GetResolution) // NOLINT
    {
        com_ptr<IWICBitmapFrameDecode> bitmap_frame_decoder = create_frame_decoder(L"lena8b.pgm");

        double dpi_x;
        double dpi_y;
        const hresult result{bitmap_frame_decoder->GetResolution(&dpi_x, &dpi_y)};
        Assert::AreEqual(error_ok, result);
        Assert::AreEqual(96., dpi_x);
        Assert::AreEqual(96., dpi_y);
    }

    TEST_METHOD(GetResolution_with_nullptr) // NOLINT
    {
        com_ptr<IWICBitmapFrameDecode> bitmap_frame_decoder = create_frame_decoder(L"lena8b.pgm");

        SUPPRESS_WARNING_6387_INVALID_ARGUMENT_NEXT_LINE
        const hresult result{bitmap_frame_decoder->GetResolution(nullptr, nullptr)};
        Assert::AreEqual(error_invalid_argument, result);
    }

    TEST_METHOD(GetColorContexts) // NOLINT
    {
        com_ptr<IWICBitmapFrameDecode> bitmap_frame_decoder = create_frame_decoder(L"lena8b.pgm");

        uint32_t actual_count;
        hresult result{bitmap_frame_decoder->GetColorContexts(0, nullptr, &actual_count)};
        Assert::AreEqual(error_ok, result);
        Assert::AreEqual(0U, actual_count);

        array<IWICColorContext*, 1> color_contexts{};
        result = bitmap_frame_decoder->GetColorContexts(static_cast<UINT>(color_contexts.size()), color_contexts.data(),
                                                        &actual_count);
        Assert::AreEqual(error_ok, result);
        Assert::AreEqual(0U, actual_count);
    }

    TEST_METHOD(GetMetadataQueryReader) // NOLINT
    {
        com_ptr<IWICBitmapFrameDecode> bitmap_frame_decoder = create_frame_decoder(L"lena8b.pgm");

        com_ptr<IWICMetadataQueryReader> metadata_query_reader;
        const hresult result{bitmap_frame_decoder->GetMetadataQueryReader(metadata_query_reader.put())};
        Assert::AreEqual(wincodec::error_unsupported_operation, result);
    }

    TEST_METHOD(CopyPixels) // NOLINT
    {
        com_ptr<IWICBitmapFrameDecode> bitmap_frame_decoder = create_frame_decoder(L"lena8b.pgm");

        uint32_t width;
        uint32_t height;

        check_hresult(bitmap_frame_decoder->GetSize(&width, &height));
        std::vector<BYTE> buffer(static_cast<size_t>(width) * height);

        hresult result{
            bitmap_frame_decoder->CopyPixels(nullptr, width, static_cast<uint32_t>(buffer.size()), buffer.data())};
        Assert::AreEqual(error_ok, result);

        result = bitmap_frame_decoder->CopyPixels(nullptr, width, static_cast<uint32_t>(buffer.size()), buffer.data());
        Assert::AreEqual(error_ok, result);
    }

    TEST_METHOD(IsIWICBitmapSource) // NOLINT
    {
        const com_ptr<IWICBitmapFrameDecode> bitmap_frame_decoder = create_frame_decoder(L"lena8b.pgm");

        const com_ptr<IWICBitmapSource> bitmap_source(bitmap_frame_decoder);
        Assert::IsTrue(bitmap_source.get() != nullptr);
    }

    TEST_METHOD(decode_8bit_monochrome) // NOLINT
    {
        com_ptr<IWICBitmapFrameDecode> bitmap_frame_decoder{create_frame_decoder(L"lena8b.pgm")};

        uint32_t width;
        uint32_t height;

        check_hresult(bitmap_frame_decoder->GetSize(&width, &height));
        vector<byte> buffer(static_cast<size_t>(width) * height);

        const hresult result{copy_pixels(bitmap_frame_decoder.get(), width, buffer.data(), buffer.size())};
        Assert::AreEqual(error_ok, result);

        compare("lena8b.pgm", buffer);
    }

    TEST_METHOD(decode_10bit_monochrome) // NOLINT
    {
        decode_2_byte_samples_monochrome(L"medical-10bit.pgm", "medical-10bit.pgm");
    }

    TEST_METHOD(decode_12bit_monochrome) // NOLINT
    {
        decode_2_byte_samples_monochrome(L"medical-m612-12bit.pgm", "medical-m612-12bit.pgm");
    }

    TEST_METHOD(decode_16bit_monochrome) // NOLINT
    {
        decode_2_byte_samples_monochrome(L"640_480_16bit.pgm", "640_480_16bit.pgm");
    }

private:
    void decode_2_byte_samples_monochrome(const wchar_t* filename_actual, const char* filename_expected)
    {
        com_ptr<IWICBitmapFrameDecode> bitmap_frame_decoder{create_frame_decoder(filename_actual)};

        uint32_t width;
        uint32_t height;

        check_hresult(bitmap_frame_decoder->GetSize(&width, &height));
        vector<uint16_t> buffer(static_cast<size_t>(width) * height);

        const hresult result{copy_pixels(bitmap_frame_decoder.get(), width * 2, buffer.data(), buffer.size() * 2)};
        Assert::AreEqual(error_ok, result);

        compare(filename_expected, buffer);
    }

    [[nodiscard]] com_ptr<IWICBitmapFrameDecode> create_frame_decoder(_Null_terminated_ const wchar_t* filename) const
    {
        com_ptr<IStream> stream;
        check_hresult(SHCreateStreamOnFileEx(filename, STGM_READ | STGM_SHARE_DENY_WRITE, 0, false, nullptr, stream.put()));

        com_ptr<IWICBitmapDecoder> wic_bitmap_decoder = factory_.create_decoder();
        check_hresult(wic_bitmap_decoder->Initialize(stream.get(), WICDecodeMetadataCacheOnDemand));

        com_ptr<IWICBitmapFrameDecode> bitmap_frame_decode;
        check_hresult(wic_bitmap_decoder->GetFrame(0, bitmap_frame_decode.put()));

        return bitmap_frame_decode;
    }

    [[nodiscard]] static com_ptr<IWICImagingFactory> imaging_factory()
    {
        com_ptr<IWICImagingFactory> imaging_factory;
        check_hresult(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory,
                                       imaging_factory.put_void()));

        return imaging_factory;
    }

    static hresult copy_pixels(IWICBitmapFrameDecode * decoder, uint32_t stride, void* buffer, size_t buffer_size)
    {
        return decoder->CopyPixels(nullptr, stride, static_cast<uint32_t>(buffer_size), static_cast<BYTE*>(buffer));
    }

    constexpr static void convert_to_little_endian_and_shift(span<uint16_t> samples, const uint32_t sample_shift) noexcept
    {
        transform(
            samples.begin(), samples.end(), samples.begin(),
            [sample_shift](const uint16_t sample) noexcept -> uint16_t { return _byteswap_ushort(sample) << sample_shift; });
    }

    static void compare(const char* filename, const vector<byte>& pixels)
    {
        portable_anymap_file anymap_file{filename};
        auto& expected_pixels{anymap_file.image_data()};

        for (size_t i{}; i < pixels.size(); ++i)
        {
            if (expected_pixels[i] != pixels[i])
            {
                Assert::IsTrue(false);
                break;
            }
        }
    }

    static void compare(const char* filename, const vector<uint16_t>& pixels)
    {
        portable_anymap_file anymap_file{filename};
        auto& expected_pixels{anymap_file.image_data()};

        const span<uint16_t> expected{reinterpret_cast<uint16_t*>(expected_pixels.data()),
                                      expected_pixels.size() / sizeof uint16_t};
        convert_to_little_endian_and_shift(expected, 16 - anymap_file.bits_per_sample());

        for (size_t i{}; i < pixels.size(); ++i)
        {
            if (expected[i] != pixels[i])
            {
                Assert::IsTrue(false);
                break;
            }
        }
    }

    factory factory_;
};
