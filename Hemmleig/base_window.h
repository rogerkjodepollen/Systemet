#pragma once


#include <vector>
#include <cstring>
#include <cstdint>
#include <string>


#include "stb_truetype.h"

#include <iostream>

#include "defs.h"

#include "images.h"

#include "graphics_base.h"

class BaseWindow {
	
public:

    BaseWindow(int _x,int _y, int _width, int _height, stbtt_fontinfo _font, Images* _images, FillType _fillType);
	~BaseWindow();

	virtual void resize();

	virtual void resize(int px, int py, int w, int h);

	virtual void setPadding(int _pad_left, int _pad_top, int _pad_right, int _pad_bottom);

	virtual void draw();

	virtual void keyPressed(int key) {};

	virtual void mouseMoved(double mx, double my) {};

	virtual void ControlKeyPressed(int key,int action, int mods) {};

	virtual void mouseClicked(double mx, double my, int button, int action) {};

	void add(BaseWindow* childWindow);

	BaseWindow* parentWindow = nullptr;

	GraphicsBase* root;

	uint8_t* mem = nullptr;

	stbtt_fontinfo font;

	Images* images;

	std::vector<BaseWindow*> children;

	int x = 0;

	int y = 0;

	int width = 0;

	int height = 0;

	int mem_size;

	FillType fillType;

	int pad_left = 0;

	int pad_right = 0;

	int pad_top = 0;

	int pad_bottom = 0;

	int padspace_w = 0;

	int padspace_h = 0;

};


