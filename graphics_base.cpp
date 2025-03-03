


#include "graphics_base.h"

#include <iostream>
#include <math.h>
#include <cstring> 
#include <fstream> 
#include <vector>
#include <filesystem>


#include "panel_window.h"
#include "text_window.h"
#include "scroll_window.h"
#include "images.h"

using namespace std;





// Vertex Shader Source Code
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoord;
    out vec2 TexCoord;
    void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);
        TexCoord = aTexCoord;
    }
)";

// Fragment Shader (to display the animated texture)
const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoord;
    uniform sampler2D texture1;
    void main() {
        
        vec4 pixel = texture(texture1, TexCoord);
        FragColor = vec4(pixel.rgb, 1.0);
    
    }
)";

float vertices[] = {
    // Positions    // Tex Coords
    -1.0f,  -1.0f,  0.0f, 1.0f,  // Top-left
    -1.0f,   1.0f,  0.0f, 0.0f,  // Bottom-left
     1.0f,   1.0f,  1.0f, 0.0f,  // Bottom-right
     1.0f,  -1.0f,  1.0f, 1.0f   // Top-right
};

// Quad Indices
unsigned int indices[] = {
    0, 1, 2,  // First Triangle
    0, 2, 3   // Second Triangle
};


GraphicsBase::GraphicsBase(int argc, char* argv[]) {
	
    std::filesystem::path exePath = argv[0];

    std::filesystem::path dirPath = exePath.parent_path();

    std::string sep(1, std::filesystem::path::preferred_separator);

    std::string exe_dir = dirPath.string() + sep ;

    images = new Images(exe_dir);

	mem = new uint8_t[WINDOW_WIDTH*WINDOW_HEIGHT*4];

    memset(mem,0,WINDOW_WIDTH*WINDOW_HEIGHT*4);


    std::vector<unsigned char> fontBuffer = readFontFile((exe_dir + "OpenSans.ttf").c_str());
    if (fontBuffer.empty()) {

        std::cerr << "KRIISE! Font er ikkje funnet!" << std::endl;

        return;

    }

    // Initialize font with stb_truetype
    stbtt_fontinfo font;
    if (!stbtt_InitFont(&font, fontBuffer.data(), stbtt_GetFontOffsetForIndex(fontBuffer.data(), 0))) {
        std::cerr << "Kan ikkje initailisere font!" << std::endl;
        return;
    }



    mainWindow = new PanelWindow(0,0,WINDOW_WIDTH,WINDOW_HEIGHT,font,images,FILL,180,180,180);

    mainWindow->setPadding(10,10,10,10);

    TextWindow* subTextWindow = new TextWindow(10,10,200,200,font,images,FILL,255,255,255);

    subTextWindow->root = this;

    mainWindow->add(subTextWindow);



	initGL();

	
	while (!glfwWindowShouldClose(window)) {

        drawAll(mainWindow,0,0);

		render();

		glfwPollEvents();

	}

	if (client != nullptr)
        delete client;

//WORK TO DO!



}


void GraphicsBase::drawAll(BaseWindow* curr, int wx, int wy) {

    curr->draw();

    for (int j = 0; j < curr->height ; j++) {
        for (int i = 0 ; i < curr->width ; i++) {

            int px = i + curr->x + wx;

            int py = j + curr->y + wy;

            if ((px >= 0) && (py >= 0) && (px < WINDOW_WIDTH) && (py < WINDOW_HEIGHT)) { 

                int index = (px + (py) * WINDOW_WIDTH)*4;

                int cindex = (i + j * curr->width)*4;

                mem[index] = curr->mem[cindex];
                mem[index+1] = curr->mem[cindex+1];
                mem[index+2] = curr->mem[cindex+2];

            }    
        }

    }

    for (BaseWindow* window : curr->children) {

        drawAll(window,wx+curr->x,wy+curr->y);

    }


}

void GraphicsBase::mouseClick(int button, int action) {

    double x,y;

    glfwGetCursorPos(window, &x, &y);

    sendMouseClicked(mainWindow,x,y,button,action);

}

void GraphicsBase::sendMouseMoved(BaseWindow* curr, double x, double y ) {

    curr->mouseMoved(x,y);

    for (BaseWindow* window : curr->children) {

        sendMouseMoved(window,x,y);

    }

}



void GraphicsBase::sendMouseClicked(BaseWindow* curr, double x, double y, int button, int action) {

    curr->mouseClicked(x,y,button,action);

    for (BaseWindow* window : curr->children) {

        sendMouseClicked(window,x,y,button,action);

    }

}


void GraphicsBase::mouseMove(int x, int y) {

   sendMouseMoved(mainWindow,x,y);

}


void GraphicsBase::sendCharacterPressed(BaseWindow* curr, int key) {

    curr->keyPressed(key);

    for (BaseWindow* window : curr->children) {

        sendCharacterPressed(window,key);

    }

}

void GraphicsBase::sendControlKeyPressed(BaseWindow* curr, int key, int action, int mods) {

    curr->ControlKeyPressed(key,action,mods);

    for (BaseWindow* window : curr->children) {

        sendControlKeyPressed(window, key, action,mods);

    }

}

void GraphicsBase::handleCharacterInput(int key) {
 
    sendCharacterPressed(mainWindow,key);

}

void GraphicsBase::handleControlKeyPress(int key, int action, int mods) {

    sendControlKeyPressed(mainWindow,key,action,mods);
   
}


std::vector<unsigned char> GraphicsBase::readFontFile(const char* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Failed to open font file: " << filename << std::endl;
        return {};
    }

    size_t fileSize = file.tellg();
    std::vector<unsigned char> fontBuffer(fileSize);

    file.seekg(0);
    file.read(reinterpret_cast<char*>(fontBuffer.data()), fileSize);

    return fontBuffer;
}


GraphicsBase::~GraphicsBase() {

	glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    
}



int GraphicsBase::initGL() {


    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);

    // Create a window
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Systemet", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    


    glfwSetWindowUserPointer(window, this);  // Store instance pointer
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, MouseMoveCallback);
    glfwSetCharCallback(window, charCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        return -1;
    }

    
    // Compile and link shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


	// Check for linking errors
	GLint success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
	    char infoLog[512];
	    glGetProgramInfoLog(shaderProgram, sizeof(infoLog), nullptr, infoLog);
	    std::cerr << "ERROR: Shader Program Linking Failed\n" << infoLog << std::endl;
	}

    

    // Setup VAO, VBO, EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    
    setupTexture();

    return 0;

}

void GraphicsBase::setupTexture() {

    if (glIsTexture(texture)) {
        glDeleteTextures(1, &texture);
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);


    uint8_t* buffer = (uint8_t*)malloc(WINDOW_WIDTH*WINDOW_HEIGHT*4);

    memset(buffer,0xFF,WINDOW_WIDTH*WINDOW_HEIGHT*4);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

    free(buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

}

void GraphicsBase::render() {

    glClear(GL_COLOR_BUFFER_BIT);

    // Update texture on GPU
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, mem);


    // Bind texure and draw quad
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    


}

void GraphicsBase::handleResize(int _width, int _height) {

    WINDOW_WIDTH = _width;

    WINDOW_HEIGHT = _height;

    if (mem) delete[] mem;

    mem = new uint8_t[WINDOW_WIDTH*WINDOW_HEIGHT*4]();

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    mainWindow->resize(0,0,WINDOW_WIDTH,WINDOW_HEIGHT);

    recurseResize(mainWindow);

    setupTexture();

    drawAll(mainWindow,0,0);

    render();

}

void GraphicsBase::recurseResize(BaseWindow* curr) {

    curr->resize();

    for (BaseWindow* window : curr->children) {

         recurseResize(window);

    }

}
