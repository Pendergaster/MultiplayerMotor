#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#include <stdio.h>
#include <stdlib.h>
#include <glad/include/glad/glad.h>
#include <glad/src/glad.c>
#include <GLFW/glfw3.h>
#include "math.h"
#include "inputs.h"
#include "renderer.h"
#include "camera.h"
#include "game.h"

static void glfw_error_callback(int e, const char *d) {
	printf("Error %d: %s\n", e, d);
}

GLFWwindow* init_window() 
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE  );
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
	Game game;
	init_inputs(&game.inputs);
	glfwSetWindowUserPointer(window,&game.inputs);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
	Camera camera = get_camera(
			{0.f,0.f,-3.f},
			0,-90,
			90.f,	
			(float)SCREENWIDHT / (float)SCREENHEIGHT
			);
	init_game(&game);
	game.renderer = init_renderer();

	const double dt = 1.0 / 60.0;
	double currentTime = glfwGetTime();
	double accumulator = 0.0;

	while (!glfwWindowShouldClose(window)) {
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		{
			break;
		}
		glfwPollEvents();

		if(key_pressed(Key::KEY_E)) { LOG("e pressed\n"); }
		else if (key_down(Key::KEY_E)) { LOG("e down"); }

		int display_w,display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClear(GL_COLOR_BUFFER_BIT);
		// timesteps
		double newTime = glfwGetTime();
		double frameTime = newTime - currentTime;
		currentTime = newTime;
		accumulator += frameTime;
		while(accumulator >= dt)
		{
			accumulator -= dt;
			render_start(&game.renderer);
			update_camera(&camera);
			update_components(&game);
			update_keys(&game.inputs);
		}
		/*End of the test*/
		render(&game.renderer,camera);
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	printf("bye!\n");
	return 0;
}

