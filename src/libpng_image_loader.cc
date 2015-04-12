//
//  libpng_image_loader.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 12/22/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#include <rp/ui/libpng_image_loader.h>
#include <rp/ui/image.h>
#include <rp/ui/format_util.h>
#include <rp/ui/scopes.h>
#include <rp/util/buffer.h>
#include <rp/util/scopes.h>
#include <rp/util/endian.h>
#include <string.h>

extern "C" {
#include <png.h>
#include <pngstruct.h>
}

using namespace std;
using namespace rp::util;

namespace rp { namespace ui {
    
    LibpngImageLoader::LibpngImageLoader() {}
    LibpngImageLoader::~LibpngImageLoader() {}
    
    bool LibpngImageLoader::canLoad(std::shared_ptr<rp::util::Buffer> buffer) {
        BufferLockScope scope(buffer);
        return isPng(scope.getBuffer(), buffer->size());
    }
    
    ImageMetadata LibpngImageLoader::loadMetadata(std::shared_ptr<rp::util::Buffer> buffer) {
        BufferLockScope scope(buffer);
        ImageMetadata metadata = {
            -1, //.width
            -1, //.height
            PixelFormatInvalid //.pixelFormat
        };
        
        
        if (!isPng(scope.getBuffer(), buffer->size())) {
            return metadata;
        }
        
        PngIhdrHeader* header = reinterpret_cast<PngIhdrHeader*>((char*)scope.getBuffer()+PngIhdrHeaderOffset);
        
        metadata.width = be32_to_cpu(header->width);
        metadata.height = be32_to_cpu(header->height);
        metadata.pixelFormat = header->colorType == 6 ? PixelFormatRGBA8 : PixelFormatRGB8;
        
        return metadata;
    }

    static void pngReadBytes(png_structp png, png_bytep outBytes, png_size_t byteCountToRead) {
        memcpy(outBytes, png->io_ptr, byteCountToRead);
        png->io_ptr = ((char*)png->io_ptr)+byteCountToRead;
    }
    
    template<typename T>
    static inline T min(T a, T b) {
        return (a>b) ? b : a;
    }
    
    shared_ptr<Image> LibpngImageLoader::loadImage(std::shared_ptr<rp::util::Buffer> buffer) {
        ImageMetadata metadata = loadMetadata(buffer);
        
        if (metadata.width < 0 || metadata.height < 0 || metadata.pixelFormat == PixelFormatInvalid)
            return nullptr;
        
        BufferLockScope bufferLockScope(buffer);
        
        png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
        if (!png) return nullptr;
        
        png_infop info = png_create_info_struct(png);
        if (!info) {
            png_destroy_read_struct(&png, 0, 0);
            return nullptr;
        }
        
        png_set_read_fn(png, bufferLockScope.getBuffer(), pngReadBytes);
        
        png_read_info(png, info);
        
        png_uint_32 w = 0, h = 0;
        int bitDepth = 0;
        int colorType = -1;
        
        png_get_IHDR(png, info, &w, &h, &bitDepth, &colorType, 0, 0, 0);
        
        const png_size_t bytesPerRow = png_get_rowbytes(png, info);
        const size_t bytesPerPixel = metadata.pixelFormat == PixelFormatRGB8 ? 3 : 4;
        char* rowData = new char[bytesPerRow];
        
        shared_ptr<Image> image(new Image(metadata.width, metadata.height, metadata.pixelFormat));
        ImageLockScope<char> imageLockScope(image);
        
        char* outRow = imageLockScope.getBuffer();
        for (int y = 0; y < min((int)h, metadata.height); y++, outRow += metadata.width*bytesPerPixel) {
            png_read_row(png, (png_bytep)rowData, 0);
            memcpy(outRow, rowData, min(metadata.width*bytesPerPixel, bytesPerRow));
        }
        
        delete[] rowData;
        png_destroy_read_struct(&png, &info, 0);
        
        return image;
    }
    
}}
