#pragma once

#include <cstdint>


const int BARW = 16;
const int TOPMENUH = 48;

const char* const IMAGES[] = {"Gfx/save.bmp","Gfx/server.bmp"};
const int NUM_IMAGES = 2;

enum FillType {

	STATIC,
	FILL

};

struct image_data {

	uint8_t* raw_data;

	int width;

	int height;

};



struct text_data {

	int letter;

	bool newline;

	int color;

	bool underlined;

	bool italic;

	int fontsize;

	int font;

	bool selected;

};

struct control_data {

	int xpos;
	int ypos;
	int w;
	int h;

};


#pragma pack(push, 1)
struct BITMAPFILEHEADER {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};

struct BITMAPINFOHEADER {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#pragma pack(pop)