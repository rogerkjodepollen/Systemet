
#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdint>

#include <vector>

#include "stb_truetype.h"

#include "images.h"

#include "client.h"

class PanelWindow;

class BaseWindow;

class GraphicsBase {
	
public:


	GraphicsBase(int argc, char* argv[]);

	~GraphicsBase();

    int initGL();

    void render();

    void setupTexture();

    void drawAll(BaseWindow* curr, int wx, int wy);

	void mouseClick(int button, int action);

    void sendMouseClicked(BaseWindow* curr, double x, double y, int button, int action);

	void mouseMove(int x, int y);

    void sendMouseMoved(BaseWindow* curr, double x, double y );

	void sendCharacterPressed(BaseWindow* curr, int key);

    void sendControlKeyPressed(BaseWindow* curr, int key, int action,int mods);

	void handleCharacterInput(int key);

    void handleControlKeyPress(int key, int action, int mods);

    void handleResize(int _width, int _height);

    void recurseResize(BaseWindow* curr);


	std::vector<unsigned char> readFontFile(const char* filename);

	uint32_t* GenerateAnimatedTexture(int width, int height, float time);

	uint8_t* mem = nullptr;

	GLFWwindow* window;

    Images* images;

    Client* client;

private:

    

	GLuint shaderProgram;	

	GLuint VAO, VBO, EBO;

	GLuint texture = -1;

	PanelWindow* mainWindow;

	int WINDOW_WIDTH = 640;
    int WINDOW_HEIGHT = 420;

	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        GraphicsBase* instance = static_cast<GraphicsBase*>(glfwGetWindowUserPointer(window));
        if (instance) {
            instance->mouseClick(button, action);
        }
    }

	static void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
        GraphicsBase* instance = static_cast<GraphicsBase*>(glfwGetWindowUserPointer(window));
        if (instance) {
            instance->mouseMove(xpos, ypos);
        }
    }

	static void charCallback(GLFWwindow* window, unsigned int codepoint) {
        GraphicsBase* instance = static_cast<GraphicsBase*>(glfwGetWindowUserPointer(window));
        if (instance) {
            instance->handleCharacterInput(codepoint);
        }
    }

    // GLFW callback for keys like Backspace
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        GraphicsBase* instance = static_cast<GraphicsBase*>(glfwGetWindowUserPointer(window));
        if (instance) {
            instance->handleControlKeyPress(key, action,mods);
        }
    }

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        // Retrieve the instance of GraphicsBase stored in the window user pointer
        GraphicsBase* instance = static_cast<GraphicsBase*>(glfwGetWindowUserPointer(window));
        if (instance) {
            instance->handleResize(width, height); // Call the instance method
        }
    }

};


