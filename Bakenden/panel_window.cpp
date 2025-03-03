

#include "panel_window.h"



PanelWindow::PanelWindow( int _x, int _y, int _width, int _height, stbtt_fontinfo _font, Images* _images,FillType _fillType, uint8_t _r, uint8_t _g, uint8_t _b) : BaseWindow(_x,_y,_width,_height,_font,_images,_fillType) , r(_r) , g(_g) , b(_b) {

}


void PanelWindow::draw() {

	for (int j = 0 ; j < height ; j++) {
		for (int i = 0 ; i < width ; i++) {

			int index = (i + j * width)*4;	

			mem[index] = r;
			mem[index+1] = b;
			mem[index+2] = g;
			
		} 
	}

}

