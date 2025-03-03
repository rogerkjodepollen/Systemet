

#include "topmenu_window.h"

#include "draw_tools.h"

TopMenuWindow::TopMenuWindow( int _x, int _y, int _width, int _height, stbtt_fontinfo _font, Images* _images,FillType _fillType, uint8_t _r, uint8_t _g, uint8_t _b) : BaseWindow(_x,_y,_width,_height,_font,_images,_fillType) , r(_r) , g(_g) , b(_b) {}


void TopMenuWindow::draw() {
	
	for (int j = 0 ; j < TOPMENUH ; j++) {
		for (int i = 0 ; i < width ; i++) {

			int index = (i + j*width)*4;

			mem[index] = 200;		
			mem[index+1] = 200;
			mem[index+2] = 200;

		}

	}

    images->drawImage(mem,0, width, 20, 0); 

	images->drawImage(mem,1, width, 20+80, 0); 

    DrawTools::drawHorizontalLine(mem,width ,0,0,width, 0,0,0);

	DrawTools::drawHorizontalLine(mem,width ,0,TOPMENUH-1,width, 0,0,0);

	DrawTools::drawVerticalLine(mem,width ,0,0,TOPMENUH-1, 0,0,0);

	DrawTools::drawVerticalLine(mem,width ,width-1,0,TOPMENUH-1, 0,0,0);


}

