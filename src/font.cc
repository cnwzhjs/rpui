//
//  font.cc
//  rpusbdispsdk
//
//  Created by Tony Huang on 7/3/14.
//  Copyright (c) 2014 RoboPeak.com. All rights reserved.
//

#include <rp/ui/font.h>
#include <rp/ui/image.h>
#include <rp/ui/scopes.h>
#include <rp/ui/style_manager.h>

#include <vector>

#define TAB_SIZE 4

using namespace std;

namespace rp { namespace ui {

    static FT_Library _ftLibrary = 0;
    
    static inline bool ensureFtLibrary() {
        if (_ftLibrary) return true;
        
        int error = FT_Init_FreeType(&_ftLibrary);
        if (error) {
            fprintf(stderr, "Failed to initialize freetype2...\n");
            return false;
        }
        
        return true;
    }
    
    Font::Font(const std::string& fontFace, float fontSize) {
        if (!ensureFtLibrary()) return;
        
        size_ = fontSize;
        
        auto style = StyleManager::defaultManager();
        std::string fontDir = style->getString("Font.FontDir");
        std::string fontFilename = fontDir + "/" + fontFace + ".ttf";
        
        FT_New_Face(_ftLibrary, fontFilename.c_str(), 0, &font_);
        FT_Set_Char_Size(font_, 0, (unsigned int)(fontSize*4/3*64), 72, 72);
        FT_Select_Charmap(font_, FT_ENCODING_UNICODE);
    }
    
    Font::~Font() {
        if (font_) {
            FT_Done_Face(font_);
            font_ = 0;
        }
    }

    static inline int get_length(unsigned char ch) {
        ch &= 0xf0;
        if (ch == 0xf0) return 4;
        if (ch == 0xe0) return 3;
        if (ch == 0xc0) return 2;
        return 1;
    }
    
    static inline void utf8_to_utf32(const unsigned char* utf8, unsigned long* buffer, size_t bufferSize) {
        const unsigned char* p = utf8;
        unsigned long* r = buffer;
        
        size_t size = 0;
        while (*p && size < bufferSize-1) {
            int l = get_length(*p);
            unsigned long ch;
            
            ch = *p ^ "\0\xc0\xe0\xf0"[l-1];
            p++;
            
            for (int i = 1; i < l && *p; i++, p++) {
                ch<<=6;
                ch |= (*p ^ 0x80);
            }
            
            if (ch == '\t') {
                for (int i = 0; i < TAB_SIZE; i++) {
                    *r = ' ';
                    r++; size++;
                }
            } else if (ch == '\r') {
                continue;
            } else {
                *r = ch;
                r++; size++;
            }
        }
        
        buffer[size] = 0;
    }
    
#define MAX_STRING  65535
    
    Size Font::measure(const std::string& text) {
        unsigned long utf32Text[MAX_STRING];
        
        utf8_to_utf32((const unsigned char*)text.c_str(), utf32Text, MAX_STRING);
        
        int lineHeight = (int)(size_*64);

        long minX = 0, minY = 0;
        long maxX = 0, maxY = 0;
        long curX = 0, curY = 0;
        
        const unsigned long* p = utf32Text;
        while(*p) {
            unsigned long c = *p;
            if (c == ' ') c = 'H';
            
            if (c == '\n') {
                curY += lineHeight;
                p++;
                continue;
            }
            
            int glyphIndex = FT_Get_Char_Index(font_, c);
            FT_Load_Glyph(font_, glyphIndex, FT_LOAD_RENDER);
            
            long charMinX = curX + font_->glyph->metrics.horiBearingX;
            long charMaxX = charMinX + font_->glyph->metrics.width;
            long charMinY = curY - font_->glyph->metrics.horiBearingY;
            long charMaxY = curY + charMinY + font_->glyph->metrics.height;
            
            if (charMinX < minX) minX = charMinX;
            if (charMaxX > maxX) maxX = charMaxX;
            if (charMinY < minY) minY = charMinY;
            if (charMaxY > maxY) maxY = charMaxY;
            
            curX += font_->glyph->metrics.horiAdvance;
            
            //log(DBUG "[graphics] Font::measure: '%c' (%d,%d)-(%d,%d) advance: %d", *p, charMinX, charMinY, charMaxX, charMaxY, font_->glyph->metrics.horiAdvance);
            p++;
        }
        
        long width = 4+(float)(maxX-minX)/64.0f;
        long height = 4+(float)(maxY-minY)/64.0f;
        
        return Size((int)width, (int)height);
    }
    
    static inline void drawBitmap(unsigned char* buffer, FT_Bitmap* bitmap, int x, int y, int width, int height, unsigned char r, unsigned char g, unsigned char b) {
        unsigned char* p = bitmap->buffer;
        for(int j = 0; j < bitmap->rows; j++) {
            for(int i = 0; i < bitmap->width; i++, p++) {
                int curX = x + i + 2;
                int curY = y + j + 2;
                if (curX < 0 || curX >= width) continue;
                if (curY < 0 || curY >= height) continue;
                
                int pixelIndex = curY*width+curX;
                
                unsigned char* o = buffer + pixelIndex*4;
                if (*p) {
                    o[0] = r;
                    o[1] = g;
                    o[2] = b;
                    o[3] = *p;
                } else {
                    *(unsigned int*)o = 0;
                }
            }
        }
    }
    
    shared_ptr<Image> Font::render(const std::string &text, rp::ui::Color color) {
        return render(text, color, measure(text));
    }

    struct LineInfo {
        LineInfo() : baseY(0), baseCurX(0), contentWidth(0), isFull(false) {}
        
        std::vector<unsigned long> characters;
        long baseY;
        long baseCurX;
        long contentWidth;
        bool isFull;
    };

    shared_ptr<Image> Font::render(const std::string &text, rp::ui::Color color, rp::ui::Size size) {
        if (size.width <= 0 || size.height <= 0) {
            return nullptr;
        }
        
        int bufferWidth = size.width;
        int bufferHeight = size.height;
        
        shared_ptr<Image> outputImage(new Image(size.width, size.height, PixelFormatRGBA8));
        ImageLockScope<uint32_t> lockScope(outputImage);
        
        unsigned char* buffer = (unsigned char*)lockScope.getBuffer();
        memset(buffer, 0, bufferWidth*bufferHeight*4);
        
        unsigned char r = color.r;
        unsigned char g = color.g;
        unsigned char b = color.b;
        
        unsigned long utf32Text[MAX_STRING];
        utf8_to_utf32((const unsigned char*)text.c_str(), utf32Text, MAX_STRING);
        
        // Break into lines
        std::vector<LineInfo> lines;
        LineInfo lineInfo;
        
        unsigned long* p = utf32Text;
        int curX = 0;
        int curY = 0;
        int lineHeight = (int)(size_*1.5*64);
        lineInfo.baseY = 0;
        
        while(*p) {
            bool feedNewLine = false;
            
            if (*p == '\n') {
                p++;
                feedNewLine = true;
            } else {
                int glyphIndex = FT_Get_Char_Index(font_, *p);
                FT_Load_Glyph(font_, glyphIndex, FT_LOAD_RENDER);
                
                long charMinX = curX + font_->glyph->metrics.horiBearingX;
                long charMaxX = charMinX + font_->glyph->metrics.width;
                long charMinY = - font_->glyph->metrics.horiBearingY;
                
                if (lineInfo.characters.size() && charMaxX + lineInfo.baseCurX > bufferWidth*64) {
                    feedNewLine = true;
                    lineInfo.isFull = true;
                } else {
                    if (charMinX < lineInfo.baseCurX) lineInfo.baseCurX=charMinX;
                    if (charMaxX > lineInfo.contentWidth) lineInfo.contentWidth = charMaxX;
                    if (charMinY < lineInfo.baseY) lineInfo.baseY = charMinY;
                    
                    lineInfo.characters.push_back(*p);
                    curX += font_->glyph->metrics.horiAdvance;
                }
            }
            
            if (feedNewLine) {
                lineInfo.contentWidth -= lineInfo.baseCurX;
                lineInfo.baseY = curY - lineInfo.baseY;
                lines.push_back(lineInfo);
                lineInfo = LineInfo();
                curY += lineHeight;
                lineInfo.baseY = curY;
                curX = 0;
            } else {
                p++;
            }
        }
        
        if (lineInfo.characters.size()) {
            lineInfo.contentWidth -= lineInfo.baseCurX;
            lineInfo.baseY = curY - lineInfo.baseY;
            lines.push_back(lineInfo);
        }
        
        // Render texts
        for (size_t l = 0; l < lines.size(); l++) {
            LineInfo line = lines[l];
            long curX = -line.baseCurX;
            long curY = line.baseY;
            long additionalCharSpace = 0;
            
            if (line.isFull && line.characters.size() > 1) {
                additionalCharSpace = (bufferWidth*64 - line.contentWidth) / (line.characters.size()-1);
            }
            
            for (size_t i = 0; i < line.characters.size(); i++) {
                FT_Load_Char(font_, line.characters[i], FT_LOAD_RENDER);
                
                FT_Bitmap* bitmap = &font_->glyph->bitmap;
                
                long startX = curX + font_->glyph->metrics.horiBearingX;
                long startY = curY - font_->glyph->metrics.horiBearingY;
                
                startX >>= 6; startY >>= 6;
                drawBitmap(buffer, bitmap, (int)startX, (int)startY, bufferWidth, bufferHeight, r, g, b);
                //log(DBUG "[graphics] Font::render drawing character '%c' at (%d,%d)", (char)line.characters[i], startX, startY);
                
                curX += font_->glyph->metrics.horiAdvance + additionalCharSpace;
            }
        }
        
        //fprintf(stderr, "Rendered text %s of %dx%d...\n", text.c_str(), size.width, size.height);
        return outputImage;
    }
    
}}
