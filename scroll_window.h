#pragma once

#include "base_window.h"

#include "images.h"

class ScrollWindow: virtual public BaseWindow 
{
	
public:

	ScrollWindow(int _x, int _y, int _width, int _height, stbtt_fontinfo _font, Images* _images,FillType _fillType, uint8_t _r, uint8_t _g, uint8_t _b);

	void draw() override;

	uint8_t sr,sg,sb;

	int slider_height;

	int slider_pos = 0;

};