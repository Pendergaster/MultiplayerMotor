#ifndef MP_INPUTS
#define MP_INPUTS
#include "utils.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

enum class Key : unsigned int
{
	KEY_A = 1 << 0/*= GLFW_KEY_A*/,
	KEY_B = 1 << 1,
	KEY_C = 1 << 2,
	KEY_D = 1 << 3,
	KEY_E = 1 << 4,
	KEY_F = 1 << 5,
	KEY_G = 1 << 6,
	KEY_H = 1 << 7,
	KEY_I = 1 << 8,
	KEY_J = 1 << 9,
	KEY_K = 1 << 10,
	KEY_L = 1 << 11,
	KEY_M = 1 << 12,
	KEY_N = 1 << 13,
	KEY_O = 1 << 14,
	KEY_P = 1 << 15,
	KEY_Q = 1 << 16,
	KEY_R = 1 << 17,
	KEY_S = 1 << 18,
	KEY_T = 1 << 19,
	KEY_U = 1 << 20,
	KEY_V = 1 << 21,
	KEY_W = 1 << 22,
	KEY_X = 1 << 23,
	KEY_Y = 1 << 24,
	KEY_MAX = 1 << 30,
	//max_keys
};

struct Input
{
	u32			keys;
	u32			lastkeys;
	glm::vec2	mpos;
	glm::vec2	lastmpos;
	bool		mousebuttons[2];
	bool		lastmousebuttons[2];
	bool		inputDisabled;
};

void set_key_down(int key,Input* in)
{
	int realKey = key - GLFW_KEY_A + 1;
	if(BETWEEN(0,realKey,30))
	{
		BIT_SET(in->keys, (1 << (realKey - 1)));
	}
}
void set_key_up(int key,Input* in)
{
	int realKey = key - GLFW_KEY_A + 1;
	if (BETWEEN(0, realKey, 30)) {
		BIT_UNSET(in->keys, (1 << (realKey - 1)));
	}
}

void key_callback(GLFWwindow* window, int key, int , int action, int )
{
	Input* in = (Input*)glfwGetWindowUserPointer(window);
	if (action == GLFW_PRESS) {
		set_key_down(key,in);
	}
	else if (action == GLFW_RELEASE) {
		set_key_up(key,in);
	}
}

void mouse_callback(GLFWwindow* window, int button, int action, int )
{
	Input* in = (Input*)glfwGetWindowUserPointer(window);
	if(button == GLFW_MOUSE_BUTTON_LEFT) {
		if(action == GLFW_PRESS) {
			in->mousebuttons[0] = true;
		}
		else if(action == GLFW_RELEASE) {
			in->mousebuttons[0] = false;
		}
	} else if(button == GLFW_MOUSE_BUTTON_RIGHT) {
		if(action == GLFW_PRESS) {
			in->mousebuttons[1] = true;
		}
		else if(action == GLFW_RELEASE) {
			in->mousebuttons[1] = false;
		}
	} 
}
void cursor_position_callback(GLFWwindow* window,double xpos , double ypos)
{
	Input* in = (Input*)glfwGetWindowUserPointer(window);
	in->mpos = glm::vec2((float)xpos,(float)ypos);	
}

static Input* g_inputs = NULL;
static void set_input_context(Input* inputs)
{
	ASSERT_MESSAGE(!g_inputs,"Inputs already set!");
	g_inputs = inputs;
}
void init_inputs(Input* in) 
{
	in->keys = 0;
	in->lastkeys = 0;
	in->lastmpos = glm::vec2(0,0);
	in->mpos = glm::vec2(0,0);
	in->lastmousebuttons[0] = false;
	in->lastmousebuttons[1] = false;
	in->mousebuttons[0] = false;
	in->mousebuttons[1] = false;
	set_input_context(in);
}
void update_keys(Input* in)
{
	in->lastkeys = in->keys;
	in->lastmpos = in->mpos;
}
// USER SPACE
bool key_pressed(Key key)
{
	return	BIT_CHECK(g_inputs->keys, (u32)key) && !(BIT_CHECK(g_inputs->lastkeys, (u32)key));	
}
bool key_down(Key key)
{
	return BIT_CHECK(g_inputs->keys,(u32)key);
}

static glm::vec2 get_mouse_position()
{
	return g_inputs->mpos;
}

static glm::vec2 get_last_mouse_position()
{
	return g_inputs->lastmpos;
}

static glm::vec2 get_mouse_movement()
{
	return g_inputs->mpos - g_inputs->lastmpos;
}


#endif
