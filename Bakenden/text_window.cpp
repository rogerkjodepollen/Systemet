
#include "text_window.h"

#include <iostream>

#include <GLFW/glfw3.h>

#include "draw_tools.h"

#include "client.h"


TextWindow::TextWindow(int _x, int _y, int _width, int _height, stbtt_fontinfo _font, Images* _images,FillType _fillType, uint8_t _r, uint8_t _g, uint8_t _b) : BaseWindow(_x,_y,_width,_height,_font,_images,_fillType), TopMenuWindow(_x,_y,_width,_height,_font,_images,_fillType, _r , _g , _b), ScrollWindow(_x,_y,_width,_height,_font,_images,_fillType, _r , _g , _b)  {

	text.clear();

	control.clear();


	text_data new_letter;

	new_letter.newline = true;

    text.push_back(new_letter);

	control_data new_control;

	control.push_back(new_control);


	lastTime = std::chrono::high_resolution_clock::now();

}


void TextWindow::draw() {

	for (int j = TOPMENUH ; j < height  ; j++) {
		for (int i = 0 ; i < (width - BARW) ; i++) {

			int index = (i + j * width)*4;	

			mem[index] = r;
			mem[index+1] = b;
			mem[index+2] = g;
			
		} 
	}

	renderText(text,control,left_padding,top+TOPMENUH,120);

	auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = now - lastTime;

    if (elapsed.count() >= 0.4) {

		lastTime = now;

		blink = !blink;

	}

	if (blink) {

		int index = 0;

		if (control.size() > 0) {
			
			index = (control[cursor].xpos + (control[cursor].ypos - 22) * width)*4;

		} else {

			if ((control.size() == 0) || (cursor == 0)) {

				index = (left_padding + (top) * width)*4;

			} else {

				index = (left_padding + (control[cursor+1].ypos - 22) * width)*4;

			}


		}

		for (int i = 0; i < 22 ; i++) {

			index += width * 4;

			if ((index < 0) || (index >= width*height*4))
				return;

			mem[index] = 0;
			mem[index+1] = 0;
			mem[index+2] = 0;

		}

	}	

	DrawTools::drawHorizontalLine(mem,width ,0,height-1,width, 0,0,0);

	DrawTools::drawVerticalLine(mem,width ,0,0,height, 0,0,0);

	DrawTools::drawVerticalLine(mem,width ,width-1,0,height, 0,0,0);

	DrawTools::drawVerticalLine(mem,width ,width-BARW,0,height, 0,0,0);

	ScrollWindow::draw();

	TopMenuWindow::draw();

}


void TextWindow::keyPressed(int key) {

	text_data new_letter;

	new_letter.letter = key;

	new_letter.newline = false;

	new_letter.selected = false;

	if ((text.size() != 0) && (cursor < (int)text.size())) {
		
		text.insert(text.begin() + cursor,new_letter);

	} else {

		text.push_back(new_letter);

	}


	control_data new_control;

	
	if ((control.size() != 0) && (cursor < (int)control.size())) {
		
		control.insert(control.begin() + cursor,new_control);

	} else {

		control.push_back(new_control);

	}

	cursor++;


}

void TextWindow::ControlKeyPressed(int key,int action, int mods) {


	if (key == GLFW_KEY_C && (mods & GLFW_MOD_CONTROL) && action == GLFW_PRESS) {

		clip.clear();

		if (select_start < select_end) {

			for (int i = select_start; i <= select_end ; i++ ) {

				clip.push_back(text[i]);

			}

		} else {

			for (int i = select_end; i <= select_start ; i++ ) {

				clip.push_back(text[i]);

			}
		}

		for (int i = 0; i < text.size() ; i++) {

			text[i].selected = false;

		}
    
    }   

    if (key == GLFW_KEY_V && (mods & GLFW_MOD_CONTROL) && action == GLFW_PRESS) {

   		text.insert(text.begin() + cursor,clip.begin(),clip.end());

	}

	if ((action == GLFW_PRESS) || (action == GLFW_REPEAT)) {

		if (key == GLFW_KEY_BACKSPACE) {
	    
			if (cursor == 0)
				return;

			text.erase(text.begin() + cursor - 1);	
			control.erase(control.begin() + cursor -1);

			cursor--;

	   } else if (key == GLFW_KEY_LEFT) {

	    	if (cursor != 0)
	    		cursor--;

	    } else if (key == GLFW_KEY_RIGHT) {

	    	if (cursor < ((int)text.size()-1))
	    		cursor++;

	    }

	}

	if (action == GLFW_PRESS) {

	    if (key == GLFW_KEY_ENTER) {

			text_data new_letter;

			new_letter.newline = true;

			if ((text.size() != 0) && (cursor < (int)text.size())) {
			
				text.insert(text.begin() + cursor,new_letter);

			} else {

				text.push_back(new_letter);

			}


			control_data new_control;

			
			if ((control.size() != 0) && (cursor < (int)control.size())) {
				
				control.insert(control.begin() + cursor ,new_control);

			} else {

				control.push_back(new_control);

			}

			cursor++;
		
		} else  if (key == GLFW_KEY_UP) {

	    	int first_pos = findLineShiftBackwards(cursor);
	    	
			if (text[first_pos-1].newline) {
	    		
	    		cursor = first_pos;
	    		
	    		return;

	    	}

	    	if (first_pos > 0) {

		    	int above_pos = findLineShiftBackwards(first_pos - 1);

		    	

		   	
		    	int len = measureLength(first_pos,cursor);

		    	int newpos = findNewpos(above_pos,len);

		    	int len2 = measureLength(above_pos,newpos);

		    	std::cout << "cursor: " << cursor << " first_pos: " << first_pos << " above_pos: " << above_pos << " Len: " << len << " newpos: " << cursor << " Len2: " << len2 << std::endl;

		    	cursor = newpos;

		    }

	    } else if (key == GLFW_KEY_DOWN) {

	    	int first_pos = findLineShiftBackwards(cursor);

	    	int below_pos = findLineShiftForwards(cursor);

	    	


	    	if (((below_pos+1) < (text.size())) && text[below_pos+1].newline) {

	    		cursor = below_pos;

	    		return;

	    	}


	    	if (below_pos < text.size()) {

	    		int len = measureLength(first_pos,cursor);

	    		int newpos = findNewpos(below_pos,len);

	    		cursor = newpos;

	    	}

	    }
	}	    
}

int TextWindow::findLineShiftBackwards(int pos) {

	int p = pos;

	do {

		p--;

	} while ((p > 0) &&(!text[p].newline));

	//if (text[p].newline) 
//		p++;

	return p;

}

int TextWindow::findLineShiftForwards(int pos) {

	int p = pos;

	do {

		p++;

	} while ((p < text.size()) && (!text[p].newline));

	
	return p;

}

int TextWindow::measureLength(int start,int end) {

	int len = 0;

	int p = start;

	do {

		len += control[p].w;

		p++;

	} while((p < text.size()) && (p < end));

	return len;

}

int TextWindow::findNewpos(int pos,int len) {

	int sum = 0;

	int p = pos;

	do {

		sum += control[p].w;

		p++;

	} while((p < text.size()) && (!text[p].newline) && (sum < len));

	if ((p < text.size()) && (text[p].newline)) 
		p--;

	return p;

} 

void TextWindow::mouseMoved(double mx, double my) {

	mx -= x;
	my -= y;

	if (select_on) {

		int hit = hitText(mx,my);

		if (hit == -1)
			return;

		for (int i = 0; i < text.size() ; i++) {

			text[i].selected = false;

		}

		if (hit < select_start) {

			for (int i = hit; i <= select_start; i++ ) {

				text[i].selected = true;

			}

		} else {

			for (int i = select_start; i <= hit; i++ ) {

				text[i].selected = true;

			}
		}

		select_end = hit;

	}
}


void TextWindow::mouseClicked(double mx, double my, int button, int action) {

	mx -= x;
	my -= y;

    start_x = mx;
    
    start_y = my;

    int hit = hitText(mx,my);

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
    
        if (action == GLFW_PRESS) {

        	if (hit != -1) {

        		select_on = true;

				for (int i = 0; i < text.size() ; i++) {

					text[i].selected = false;

				}

				cursor = hit;

        		select_start = hit;

        	}

        } else if (action == GLFW_RELEASE) {

        	
        	select_on = false;

        }

    }


    if ((action == GLFW_PRESS) && (mx > 100)  && (my > 0) && (mx < (100+TOPMENUH)) && (my < TOPMENUH)) {


		root->client = new Client();

		root->client->connectToServer("roger","proger");


	}

}


int TextWindow::hitText(int mx, int my) {

	for (int i = 0 ; i < (int)control.size() ; i++) {
/*
		if ((mx < left_padding) && (my < control[i].ypos) && (my > (control[i].ypos - control[i].h)) && !text[i].newline) {

			return  (i - 1);

		}
*/
		if ((mx > control[i].xpos) && (my < control[i].ypos) && (mx < (control[i].xpos + control[i].w)) && (my > (control[i].ypos - control[i].h))) {

			return i;

		}

	}

	return -1;

}


void TextWindow::renderText(std::vector<text_data>& text, std::vector<control_data>& control, int px, int py, int font_size) {


    float scale = stbtt_ScaleForPixelHeight(&font, font_size);

	int advanceWidth, leftBearing; 	
	stbtt_GetCodepointHMetrics(&font, 'A', &advanceWidth, &leftBearing);

	advanceWidth *= scale*0.5;


    // Get font vertical metrics for baseline alignment
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
    int baseline = py + ascent * scale;  // Adjust baseline properly

    int xCursor = px;  // Start X position

    int lineHeight = (ascent - descent + lineGap) * scale * 0.7;

    int f_height = 0;
    int f_width = 0;
   
    unsigned char* bitmap = nullptr;

    // Now render the text with equidistant letter spacing
    for (int i = 0 ; i < (int)text.size() ; i++) {

    	


        if (!text[i].newline && (text[i].letter != 32)) {

            int xOffset, yOffset;
            bitmap = stbtt_GetCodepointBitmap(&font, 0, scale, text[i].letter, &f_width, &f_height, &xOffset, &yOffset);
            if (!bitmap) continue;

	            if (text[i].selected) {

				for (int j = 0 ; j < lineHeight ; j++ ) {
	    			for (int i = 0 ; i < f_width ; i++ ) {

		    			int indx = (i + xCursor + (j - lineHeight + baseline)*width)*4;

		    			mem[indx] = 200;	
						mem[indx+1] = 200;
						mem[indx+2] = 200;

		    		}
		    	}
		    }

            // Render character with maximum width
            for (int y = 0; y < f_height; ++y) {
                for (int x = 0; x < f_width; ++x) {
                    unsigned char alpha = bitmap[y * f_width + x];  // Font alpha value

                    if (alpha > 0) {  // Process only non-transparent pixels
                      
                        int index = (((baseline + y + yOffset) * width) + (xCursor + x + xOffset)) * 4;  // 4 bytes per pixel (RGBA)

                        if (index >= 0 && (index < width * height * 4) && ((xCursor + x + xOffset) < width)) {
                            // Foreground color (Text: White)
                            int fg_r = 0;
                            int fg_g = 0;
                            int fg_b = 0;
                            
	                        int bg_r = mem[index];
	                        int bg_g = mem[index + 1];
	                        int bg_b = mem[index + 2];


                            // Apply alpha blending correctly
                            mem[index]   = (((fg_r * alpha) + (bg_r * (255 - alpha))) / 255); // Red
                            mem[index+1] = (((fg_g * alpha) + (bg_g * (255 - alpha))) / 255); // Green
                            mem[index+2] = (((fg_b * alpha) + (bg_b * (255 - alpha))) / 255); // Blue
                        }
                    }
                }
            }
        }

		
		control[i].w = f_width;

		control[i].ypos = baseline;
        
        control[i].h = lineHeight;
        

        


        if (text[i].newline) {
            
            baseline += lineHeight;

            control[i].w = f_width;

			control[i].xpos = xCursor;

			xCursor = px;

        } else {

			control[i].xpos = xCursor;

			if (text[i].letter != 32) {

				control[i].w = f_width;

            	xCursor += f_width;
			
			} else {

				control[i].w = advanceWidth;

				xCursor += advanceWidth;

			}

        }


		if (xCursor >= (width - BARW - f_width)) {

			baseline += lineHeight;

            control[i].w = f_width;

			xCursor = px;

        }

        
        
        
        if (bitmap != nullptr) {
            
            stbtt_FreeBitmap(bitmap, nullptr);

            bitmap = nullptr;

        }


    }
}


