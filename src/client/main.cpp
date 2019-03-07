#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#include <stdio.h>
#include <stdlib.h>
#include <glad/include/glad/glad.h>
#include <glad/src/glad.c>
//#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>
#include "math.h"
#if 0
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#endif


#include "Client.h"
#include "cppincludes.h"

#define STB_IMAGE_IMPLEMENTATION

//TODO(pate) add stbimage.h
#undef STB_IMAGE_IMPLEMENTATION

#define SCREENWIDHT 800
#define SCREENHEIGHT 800
#include "inputs.h"
#include "renderer.h"
#include "camera.h"

static void glfw_error_callback(int e, const char *d)
{
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
#if 0
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* solver = new  btSequentialImpulseConstraintSolver;
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,
			overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -4.f, 0));
#endif
	Client* connection = new Client("127.0.0.1", 60000, "Loyalisti"); //Create new connection to server;
	connection->OpenConnection(); //Let attempt to open it;
	int a, w, d, s = 0;
	//TODO(mika) vaihda noista inteistä keyn yhteen inttiin
	connection->SetVar(PLAYER_INPUT, std::vector<int*>{&w, &a, &s, &d});

	GLFWwindow* window = init_window();
	Input inputs;
	init_inputs(&inputs);
	glfwSetWindowUserPointer(window,&inputs);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
	Renderer renderer = init_renderer();
	Camera camera = get_camera(
			{0.f,0.f,-3.f},
			0,-90,
			45.f,	
			(float)SCREENWIDHT / (float)SCREENHEIGHT
			);
	float deltaAngle = 1.f * deg_to_rad;
	quaternion rotaxis({90.f,0.f,90.f},deltaAngle);
	quaternion rotation;
	while (!glfwWindowShouldClose(window)) {
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		{
			break;
		}
		connection->Update();
		glfwPollEvents();

		if(key_down(Key::KEY_A)) {  a = 1; }
		else { a = 0; }
		if(key_down(Key::KEY_D)) { d = 1; } 
		else { d = 0; }
		if(key_down(Key::KEY_W)) { w = 1; } 
		else { w = 0; }
		if (key_down(Key::KEY_S)) { s = 1; }
		else { s = 0; }

		update_camera(&camera);

		if(key_pressed(Key::KEY_E)) { LOG("e pressed\n"); }
		else if (key_down(Key::KEY_E)) { LOG("e down"); }

		update_keys(&inputs);
		int display_w,display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		//glClear(GL_COLOR_BUFFER_BIT);
		rotation *= rotaxis;
		render_cube(&renderer,
				{0,0,0},
				{1.f,1.f,1.f},
				rotation,
				//{0,0,0,1.f},
				{255,255,255,255});
		render_cube(&renderer,
				{0,-3.f,0},
				{10.f,1.f,10.f},
				{1,0,0,0},
				//{0,0,0,1.f},
				{0,0,255,255});
		render(&renderer,camera);
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	connection->CloseConnection(); //Close connection to server;
	delete connection; //Hakai the connecsjioon;
	printf("bye!\n");
	return 0;
}

