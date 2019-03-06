#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#include <stdio.h>
#include <stdlib.h>
#include <glad/include/glad/glad.h>
#include <glad/src/glad.c>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define SCREENWIDHT 800
#define SCREENHEIGHT 800

#include "inputs.h"


static void glfw_error_callback(int e, const char *d)
{
	printf("Error %d: %s\n", e, d);
}

GLFWwindow* init_window() 
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwSwapInterval(0);
	GLFWwindow* window = glfwCreateWindow(SCREENWIDHT,SCREENHEIGHT, "Tabula Rasa", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glViewport(0, 0, SCREENWIDHT, SCREENHEIGHT);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetErrorCallback(glfw_error_callback);
	return window;
}


int main(int argc,char* argv[])
{
	(void)argc;(void)argv;
	GLFWwindow* window = init_window();
	Input inputs;
	init_inputs(&inputs);
	glfwSetWindowUserPointer(window,&inputs);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glm::vec2 mpos = get_mouse_position();
		printf("update! , %f ,%f ",mpos.x,mpos.y);
		if(key_pressed(Key::KEY_E)) {
			LOG("e pressed\n");
		} else if (key_down(Key::KEY_E)) {
			LOG("e down\n");
		}

		update_keys(&inputs);
		int display_w,display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	printf("bye!\n");
	return 0;
}
