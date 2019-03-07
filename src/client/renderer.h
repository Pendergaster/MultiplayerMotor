#ifndef MP_RENDERER
#define MP_RENDERER
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION
#include <glad/include/glad/glad.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/quaternion.hpp> 
#include "math.h"
#include <vector>
#include "utils.h"
#include "fileutils.h"
#include "camera.h"
#define PROJECTIONLOC 1
#define VIEWLOC 2
#define MODELLOC 3
#define COLORLOC 4
GLenum glCheckError_(const char *file, int line,const char* func = NULL)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		char* error = NULL;
		switch (errorCode) {
			case GL_INVALID_ENUM:                  error = (char*)"INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 error = (char*)"INVALID_VALUE"; break;
			case GL_INVALID_OPERATION:             error = (char*)"INVALID_OPERATION"; break;
												   //case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
												   //case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY:                 error = (char*)"OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error = (char*)"INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		if(func) {
			printf("GL ERROR %s, IN FUNCTION %s,LINE %d , FILE %s \n", error, func,line, file);
		} else {
			printf("GL ERROR %s, LINE %d , FILE %s \n", error, line, file);
		}
		ABORT_MESSAGE("GL_MISTAKE \n");
	}
	return errorCode;
}
#define glCheckError(...) glCheckError_(__FILE__, __LINE__,__VA_ARGS__)

#define GLERRCHECK(func) do{func; glCheckError(#func);}while(0)

struct Color {
	Color(u8 _r, u8 _g,u8 _b, u8 _a) : r(_r),g(_g),b(_b),a(_a) {}
	u8 r,g,b,a;
};

struct ModelData {
	u32 vertbuff;
	u32 vao;
	u32 numVerts;
};

enum class Model : i32
{
	Cube = 0,
	MaxModels
};

static ModelData load_cube() {
	ModelData cube;
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	// first, configure the cube's VAO (and cube.vertbuff)
	glGenVertexArrays(1, &cube.vao);

	GLERRCHECK(glGenVertexArrays(1, &cube.vao));

	GLERRCHECK(glGenBuffers(1, &cube.vertbuff));

	GLERRCHECK(glBindBuffer(GL_ARRAY_BUFFER, cube.vertbuff));
	GLERRCHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

	GLERRCHECK(glBindVertexArray(cube.vao));

	// position attribute
	GLERRCHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0));
	GLERRCHECK(glEnableVertexAttribArray(0));
	// normal attribute
	GLERRCHECK(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))));
	GLERRCHECK(glEnableVertexAttribArray(1));
	GLERRCHECK(glBindVertexArray(0));
	cube.numVerts = sizeof vertices / (sizeof(float) * 6);
	return cube;
}


static u32 link_shader(u32 vert, u32 frag)
{
	u32 program = glCreateProgram();
	GLERRCHECK(glAttachShader(program,vert));
	GLERRCHECK(glAttachShader(program,frag));
	GLERRCHECK(glLinkProgram(program));
	int linked = 0;
	GLERRCHECK(glGetProgramiv(program, GL_LINK_STATUS, &linked));
	if (!linked)
	{
		int infolen = 0;
		GLERRCHECK(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infolen));
		if (infolen > 1)
		{
			char* infolog = (char*)malloc(sizeof(char)*infolen);
			GLERRCHECK(glGetProgramInfoLog(program, infolen, NULL, infolog));
			LOG("Error linking program \n %s", infolog);
			free(infolog);
		}
		ABORT_MESSAGE("failed to link shader!");
		GLERRCHECK(glDeleteProgram(program));
		return 0;
	}
	GLERRCHECK(glDeleteShader(vert));
	GLERRCHECK(glDeleteShader(frag));
	return program;
}
u32 compile_shader(u32 glenum, const char* source)
{
	int compilecheck = 0;
	int shader = glCreateShader(glenum);
	ASSERT_MESSAGE(shader,"failed to create shader");

	GLERRCHECK(glShaderSource(shader, 1, &source, NULL));
	GLERRCHECK(glCompileShader(shader));
	GLERRCHECK(glGetShaderiv(shader, GL_COMPILE_STATUS, &(compilecheck)));
	if (!compilecheck)
	{
		int infolen = 0;
		GLERRCHECK(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &(infolen)));
		if (infolen > 1)
		{
			char* infoLog = (char*)malloc(sizeof(char) * infolen);
			GLERRCHECK(glGetShaderInfoLog(shader, infolen, NULL, infoLog));
			LOG("Error compiling shader : %s ", infoLog);
			free(infoLog);
			GLERRCHECK(glDeleteShader((GLuint)shader));
			ABORT_MESSAGE("failed to compile program");
			return false;
		}
	}
	return shader;
}

static u32 load_model_shader() 
{
	u32 vert,frag,prog;
	char* vertsrc = load_file("../../shaders/model.vert",NULL);
	char* fragsrc = load_file("../../shaders/model.frag",NULL);
	ASSERT_MESSAGE(vertsrc && fragsrc,"failed to load shader!");
	defer{free(vertsrc);};
	defer{free(fragsrc);};
	vert = compile_shader(GL_VERTEX_SHADER,vertsrc);
	frag = compile_shader(GL_FRAGMENT_SHADER,fragsrc);
	prog = link_shader(vert,frag);

	GLERRCHECK(glUseProgram(prog));


	return prog;
}

struct RenderData {
	RenderData(Model handle,vec3 pos,vec3 scale,quaternion ori,Color col) : 
		model(handle),position(pos),scale(scale), orientation(ori),color(col){}
	Model		model;
	vec3		position;
	vec3		scale;
	quaternion	orientation;
	Color		color;
};

struct Renderer {
	u32							modelShader;
	ModelData					models[(int)Model::MaxModels];
	std::vector<RenderData>		renderables;
};

static Renderer init_renderer() 
{
	Renderer rend;
	LOG("Renderer initing");
	rend.modelShader = load_model_shader();
	LOG("Shader loaded initing");
	rend.models[(int)Model::Cube] = load_cube();
	glClearColor(0.8f, 0.8f, 0.8f, 1.f);
	glEnable(GL_MULTISAMPLE);
	LOG("Renderer inited");
	return rend;
}

static inline void render_cube(Renderer* rend,const vec3& pos,const vec3& scale,
		const quaternion& orientation,Color color) 
{
	rend->renderables.emplace_back(Model::Cube,pos,scale,orientation,color);
}

static void render(Renderer* rend,const Camera& cam) 
{
	// set up
	GLERRCHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	glEnable(GL_DEPTH_TEST);
	//TODO(pate) check cull face order
	// glEnable(GL_CULL_FACE); 
	GLERRCHECK(glUseProgram(rend->modelShader));
	GLERRCHECK(glUniformMatrix4fv(PROJECTIONLOC, 1, GL_FALSE, (GLfloat*)&cam.projection));
	GLERRCHECK(glUniformMatrix4fv(VIEWLOC, 1, GL_FALSE, (GLfloat*)&cam.view));
	
	for(RenderData data : rend->renderables) 
	{
		//TODO(pate)  bind model and color
		mat4 model;
		//identify(&model);
		model *= data.orientation;
		translate(&model,data.position);
		scale(&model,data.scale);
		// bind model
		GLERRCHECK(glUniformMatrix4fv(MODELLOC, 1, GL_FALSE, (GLfloat*)&model));
		// convert color to 0 - 1 range float 
		vec4 color;
		color.x = data.color.r / 255.f;
		color.y = data.color.g / 255.f;
		color.z = data.color.b / 255.f;
		color.a = data.color.a / 255.f;
		// bind color
		GLERRCHECK(glUniform4fv(COLORLOC, 1, (float*)&color));
		ModelData modeldata = rend->models[(int)data.model];
		glBindVertexArray(modeldata.vao);
		GLERRCHECK(glDrawArrays(GL_TRIANGLES, 0, modeldata.numVerts));
	}
	rend->renderables.clear();
}

#endif //MP_RENDERER


