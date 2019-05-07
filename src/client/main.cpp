#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#include <stdio.h>
#include <stdlib.h>
#include <glad/include/glad/glad.h>
#include <glad/src/glad.c>
#include <GLFW/glfw3.h>
#if 1
#include <imgui/imgui.h>
#include <imgui/imgui.cpp>
#include <imgui/imgui_draw.cpp>
#include <imgui/imgui_widgets.cpp>
#include <imgui/imgui_demo.cpp>
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_glfw.cpp>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_opengl3.cpp>
#endif

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
	init_inputs(&game.inputs,window);
	glfwSetWindowUserPointer(window,&game.inputs);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); 
	init_game(&game);
	game.renderer = init_renderer();

	const double dt = 1.0 / 60.0;
	double currentTime = glfwGetTime();
	double accumulator = 0.0;


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 130";
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup style
	ImGui::StyleColorsDark();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::EndFrame();
	activate_cursor();
	bool breakLoop = false;
	while (!glfwWindowShouldClose(window) && !breakLoop) {
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		{
			break;
		}
		glfwPollEvents();

		if (key_pressed(Key::KEY_E)) { LOG("e pressed\n"); }
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
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			accumulator -= dt;
			render_start(&game.renderer);
			update_camera(&game.camera);
			if(!update_components(&game)) breakLoop = true;
			update_keys(&game.inputs);

			ImGui::EndFrame();
		}
		/*End of the test*/
		render(&game.renderer,game.camera);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	printf("bye!\n");
	return 0;
}

