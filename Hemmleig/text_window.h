#pragma once

#include <chrono>

#include "scroll_window.h"

#include "topmenu_window.h"

#include <vector>

#include "stb_truetype.h"

#include "defs.h"


class TextWindow : public ScrollWindow , public TopMenuWindow
{

public:

	
	TextWindow(int _x, int _y, int _width, int _height, stbtt_fontinfo _font, Images* _images,FillType _fillType, uint8_t _r, uint8_t _g, uint8_t _b);
	
	void draw() override;

	void keyPressed(int key) override;

	void ControlKeyPressed(int key,int action, int mods) override;

	void mouseMoved(double mx, double my) override;

    void mouseClicked(double mx, double my, int button, int action) override;

	void renderText(std::vector<text_data>& text, std::vector<control_data>& control, int px, int py, int font_size);

	int findLineShiftBackwards(int pos);

	int findLineShiftForwards(int pos);

	int measureLength(int start,int end);

	int findNewpos(int pos,int len);

	int hitText(int mx, int my);


	std::vector<text_data> text;

	std::vector<control_data> control;


	int top = 10;

	int left_padding = 10;

	int cursor = 0;

	bool blink = true;

	std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;


	bool select_on = false;

	int select_start = -1;

	int select_end = -1; 

	int start_x, start_y;

	std::vector<text_data> clip;

	bool left_pressed = false;

	bool left_spool = false;

	std::chrono::time_point<std::chrono::high_resolution_clock> leftTime;


};