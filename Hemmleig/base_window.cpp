

#include "base_window.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

BaseWindow::BaseWindow(int _x,int _y, int _width, int _height, stbtt_fontinfo _font, Images* _images, FillType _fillType) {
	
	x = _x;
	y = _y;

	width = _width;
	height = _height;

	images = _images;

	fillType = _fillType;

 	pad_left = 0;

	pad_top = 0;

	padspace_w = width;
    
    padspace_h = height;


	font = _font;

	mem_size = width * height * 4;

	mem = new uint8_t[mem_size];

	children.clear();

}


BaseWindow::~BaseWindow(){}

void BaseWindow::setPadding(int _pad_left, int _pad_top, int _pad_right, int _pad_bottom) {

	pad_left = _pad_left;
	pad_top = _pad_top;
	pad_right = _pad_right;
	pad_bottom = _pad_bottom;

	padspace_w = width - pad_left - pad_right;

	padspace_h = height - pad_top - pad_bottom;

}

void BaseWindow::resize(int px, int py, int w, int h) {

	if (mem != nullptr)
		free (mem);

	mem = new uint8_t[w*h*4];

	x = px;

	y = py;

	width = w;

	height = h;

	padspace_w = width - pad_left - pad_right;

	padspace_h = height - pad_top - pad_bottom;

}

void BaseWindow::resize() {

	if (parentWindow == nullptr)
		return;


	if (fillType == FILL) {
		
		x = parentWindow->pad_left;

		y = parentWindow->pad_top;

		width = parentWindow->padspace_w;

		height = parentWindow->padspace_h;

		if (mem != nullptr)
			free (mem);

		mem = new uint8_t[width*height*4];

		padspace_w = width - pad_left - pad_right;

		padspace_h = height - pad_top - pad_bottom;

	}

}



void BaseWindow::add(BaseWindow* childWindow) {

	childWindow->parentWindow = this;

	if (fillType == FILL) {

		childWindow->resize();
	
	}

	children.push_back(childWindow);

}

void BaseWindow::draw() {
	

	memset(mem,0xFF,mem_size);


}


