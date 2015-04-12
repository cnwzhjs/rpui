
//
//  format_util.h
//  Some utility methods to detect image file formats
//
//  Created by Tony Huang on 12/22/13.
//  Copyright (c) 2013 RoboPeak.com. All rights reserved.
//

#pragma once

#include <rp/infra_config.h>
#include <rp/util/int_types.h>

namespace rp { namespace ui {

#pragma pack(push, 1)
    struct PngIhdrHeader {
        uint32_t width;
        uint32_t height;
        uint8_t bitDepth;
        uint8_t colorType;
    } __attribute__ ((packed));
    
    struct JpegSofHeader {
        uint8_t signature[2];
        uint16_t sectionSize;
        uint8_t bitsPerChannel;
        uint16_t height;
        uint16_t width;
    } __attribute__ ((packed));
#pragma pack(pop)
    
    enum {
        PngIhdrHeaderOffset = 16
    };
    
    static inline bool isJpeg(void* buffer, size_t size) {
        if (size < 16) return false;
        const unsigned char* bytes = (const unsigned char*)buffer;
        return (bytes[0] == 0xffu) &&
        (bytes[1] == 0xd8u);
    }
    
    static inline bool isPng(void* buffer, size_t size) {
        if (size < (PngIhdrHeaderOffset+sizeof(PngIhdrHeader))) return false;
        const unsigned char* bytes = (const unsigned char*)buffer;
        return (bytes[0] == 0x89) && (bytes[1] == 'P') && (bytes[2] == 'N') && (bytes[3] == 'G');
    }
    
}}
