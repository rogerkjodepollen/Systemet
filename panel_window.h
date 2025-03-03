#pragma once

#include "defs.h"

#include "base_window.h"

#include "images.h"

#include "stb_truetype.h"

class PanelWindow : public BaseWindow 
{

public:	

	PanelWindow( int _x, int _y, int _width, int _height, stbtt_fontinfo _font, Images* _images,FillType _fillType, uint8_t _r, uint8_t _g, uint8_t _b);

	void draw() override;

private:

	uint8_t r,g,b;


};