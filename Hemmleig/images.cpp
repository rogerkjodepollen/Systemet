
#include "images.h"

#include <cstring> 

#include "defs.h"


Images::Images(std::string exe_dir) {
	
    image_list.clear();

    for ( int i = 0 ; i < NUM_IMAGES; i++) {

        image_data* image = loadImage(exe_dir + IMAGES[i]);

        if (image == nullptr)
            std::cout << "Failed to load imgage!" << std::endl;

        image_list.push_back(image);

    }




}

void Images::drawImage(uint8_t* mem, int nr, int width, int x, int y) {

	image_data* img = image_list[nr];

	for (int j = 0 ; j < img->height ; j++ ) {

		for (int i = 0 ; i < img->width ; i++ ) {

			int alpha = img->raw_data[(i + j * img->width)*4 + 3];


			mem[(i + x + (j + y)*width)*4] = (img->raw_data[(i + j * img->width)*4+2] * alpha + mem[(i + x + (j + y)*width)*4] * (255 - alpha)) >> 8;

			mem[(i + x + (j + y)*width)*4+1] = (img->raw_data[(i + j * img->width)*4+1] * alpha + mem[(i + x + (j + y)*width)*4+1] * (255 - alpha)) >> 8;

			mem[(i + x + (j + y)*width)*4+2] = (img->raw_data[(i + j * img->width)*4] * alpha + mem[(i + x + (j + y)*width)*4+2] * (255 - alpha)) >> 8;


		}

	}

}



image_data* Images::loadImage(const std::string& filepath) {


	image_data* new_image_data = new image_data;


    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return nullptr;
    }

    BITMAPFILEHEADER fileHeader;
    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));

    if (fileHeader.bfType != 0x4D42) {
        std::cerr << "Not a valid BMP file: " << filepath << std::endl;
        return nullptr;
    }

    BITMAPINFOHEADER infoHeader;
    file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    if (infoHeader.biBitCount != 24 && infoHeader.biBitCount != 32) {
        std::cerr << "Unsupported BMP format: Only 24-bit or 32-bit BMPs are supported." << std::endl;
        return nullptr;
    }
/*
    if (infoHeader.biCompression != 0) {
        std::cerr << "Unsupported BMP format: Only uncompressed BMPs are supported." << std::endl;
        return nullptr;
    }
*/
    new_image_data->width = infoHeader.biWidth;
    new_image_data->height = infoHeader.biHeight;

    size_t pixelDataSize = infoHeader.biSizeImage;
    if (pixelDataSize == 0) {
        size_t rowSize = (infoHeader.biBitCount * new_image_data->width + 31) / 32 * 4;
        pixelDataSize = rowSize * new_image_data->height;
    }

    new_image_data->raw_data = new uint8_t[pixelDataSize];

   
    file.seekg(fileHeader.bfOffBits, std::ios::beg);
    file.read((char*)new_image_data->raw_data, pixelDataSize);

    if (infoHeader.biHeight > 0) {
        size_t rowSize = (infoHeader.biBitCount * new_image_data->width + 31) / 32 * 4;
        std::vector<unsigned char> rowBuffer(rowSize);
        for (int y = 0; y < new_image_data->height / 2; ++y) {
            unsigned char* row1 = new_image_data->raw_data + y * rowSize;
            unsigned char* row2 = new_image_data->raw_data + (new_image_data->height - 1 - y) * rowSize;
            std::memcpy(rowBuffer.data(), row1, rowSize);
            std::memcpy(row1, row2, rowSize);
            std::memcpy(row2, rowBuffer.data(), rowSize);
        }
    }

    file.close();

    return new_image_data;

}

