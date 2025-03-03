

#include "scroll_window.h"


ScrollWindow::ScrollWindow( int _x, int _y, int _width, int _height, stbtt_fontinfo _font, Images* _images,FillType _fillType, uint8_t _r, uint8_t _g, uint8_t _b) : BaseWindow(_x,_y,_width,_height,_font,_images,_fillType) , sr(_r) , sg(_g) , sb(_b) {}




void ScrollWindow::draw() {

	

	for (int j = TOPMENUH ; j < height ; j++) {
		for (int i = 0 ; i < BARW ; i++) {

			int index = (width - BARW + i + (j)*width)*4;

			mem[index] = 128;
			mem[index+1] = 128;
			mem[index+2] = 128;
			
		}
	}

	slider_height = 100;

	for (int j = TOPMENUH ; j < slider_height ; j++) {
		for (int i = 0 ; i < BARW ; i++) {

			int index = (width - BARW + i + (j+slider_pos)*width)*4;

			mem[index] = 80;
			mem[index+1] = 80;
			mem[index+2] = 80;
			
		}
	}

}
