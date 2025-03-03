#pragma once

#include <cstdint>


class DrawTools
{

public:
	
	static void drawHorizontalLine(uint8_t* mem, int width , int x, int y, int w, uint8_t r, uint8_t g,uint8_t b);

	static void drawVerticalLine(uint8_t* mem,int width , int x, int y , int h,  uint8_t r, uint8_t g,uint8_t b);	

};