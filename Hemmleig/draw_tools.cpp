
#include "draw_tools.h"






void DrawTools::drawHorizontalLine(uint8_t* mem,int width , int x, int y , int w,  uint8_t r, uint8_t g,uint8_t b) {

	for (int i = 0 ; i < w ; i++) {

		int index = (x + i + y * width)*4;

		mem[index] = r;
		mem[index+1] = g;	
		mem[index+2] = b;

	}

}

void DrawTools::drawVerticalLine(uint8_t* mem,int width , int x, int y , int h,  uint8_t r, uint8_t g,uint8_t b) {

	for (int i = 0 ; i < h ; i++) {

		int index = (x +  (y + i) * width)*4;

		mem[index] = r;
		mem[index+1] = g;	
		mem[index+2] = b;

	}

}