#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#include <stdio.h>
#include <stdlib.h>
#include <glad/include/glad/glad.h>
#include <glad/src/glad.c>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "cppincludes.h"
#include "client.h"

#define STB_IMAGE_IMPLEMENTATION

//TODO(pate) add stbimage.h
#undef STB_IMAGE_IMPLEMENTATION
#define SCREENWIDHT 800
#define SCREENHEIGHT 800
int main(int argc,char* argv[])
{
	Client* connection = new Client("127.0.0.1", 60000, "Loyalisti"); //Create new connection to server;
	connection->OpenConnection(); //Let attempt to open it;

	(void)argc;(void)argv;
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

	while (!glfwWindowShouldClose(window))
	{
		connection->Update(); //Let's check if server have sent us a packet or two;
		glfwPollEvents();
		printf("update!\n");
		int display_w,display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	connection->CloseConnection(); //Close connection to server;
	delete connection; //Hakai the connecsjioon;
	printf("bye!\n");
	return 0;
}

