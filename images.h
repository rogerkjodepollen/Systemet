#pragma once

#include <cstdint>

#include <iostream>
#include <fstream>

#include <vector>

#include "defs.h"



class Images {
	
public:
	
	Images(std::string exe_dir);

	void drawImage(uint8_t* mem, int nr, int width, int x, int y);

	image_data* loadImage(const std::string& filepath);



	std::vector<image_data*> image_list;


};