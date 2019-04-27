#ifndef MP_RENDERER
#define MP_RENDERER
#define STB_IMAGE_IMPLEMENTATION
//#include <glm_deprecated/glm.hpp>
//#include <glm_deprecated/gtc/matrix_transform.hpp>
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#undef TINYOBJLOADER_IMPLEMENTATION
//#include <glad/include/glad/glad.h>
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
#define TEXTURELOC 5
#define SHADOWMATRIXLOC 8
#define SHADOWSPLITLOC 12
#define SHADOWSAMPLERLOC 13
#define SCREENWIDHT 800
#define SCREENHEIGHT 800

const vec3 glight_direction = vec3(-2.f,-4.f,0);
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
struct vertexdata{
	vec3 pos,normal;
};
void load_obj(const char* path,std::vector<vertexdata>* buffer) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path)) {
		ABORT_MESSAGE("Failed to load model!! %s , %s",warn.c_str(),err.c_str());
	}

	for (const tinyobj::shape_t& shape : shapes) 
	{
		for (const auto& index : shape.mesh.indices) 
		{
			vertexdata vertex = {};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.normal = {
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2],
			};
#if 0
			vertex.uv = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};
#endif
			buffer->push_back(vertex);
		}
	}
}

static ModelData load_cube() {
	ModelData cube;
#if 0
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
#else 
	std::vector<vertexdata> d;
	load_obj("models/Cube.obj",&d);
	// first, configure the cube's VAO (and cube.vertbuff)
	glGenVertexArrays(1, &cube.vao);

	GLERRCHECK(glGenVertexArrays(1, &cube.vao));

	GLERRCHECK(glGenBuffers(1, &cube.vertbuff));

	GLERRCHECK(glBindBuffer(GL_ARRAY_BUFFER, cube.vertbuff));
	GLERRCHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertexdata) * d.size(), d.data(), GL_STATIC_DRAW));

	GLERRCHECK(glBindVertexArray(cube.vao));

	// position attribute
	GLERRCHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0));
	GLERRCHECK(glEnableVertexAttribArray(0));
	// normal attribute
	GLERRCHECK(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))));
	GLERRCHECK(glEnableVertexAttribArray(1));
	GLERRCHECK(glBindVertexArray(0));
	cube.numVerts = d.size();//sizeof vertices / (sizeof(float) * 6);

#endif
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

static u32 load_shader(const char* verts,const char* frags)
{
	u32 vert,frag,prog;
	char* vertsrc = load_file(verts,NULL);
	char* fragsrc = load_file(frags,NULL);
	ASSERT_MESSAGE(vertsrc && fragsrc,"failed to load shader!");
	defer{free(vertsrc);};
	defer{free(fragsrc);};
	vert = compile_shader(GL_VERTEX_SHADER,vertsrc);
	frag = compile_shader(GL_FRAGMENT_SHADER,fragsrc);
	prog = link_shader(vert,frag);

	GLERRCHECK(glUseProgram(prog));


	return prog;
}
struct Color {
	Color(u8 _r, u8 _g,u8 _b, u8 _a) : r(_r),g(_g),b(_b),a(_a) {}
	u8 r,g,b,a;
};

struct RenderData {
	RenderData(Model handle,vec3 pos,vec3 scale,quaternion ori,Color col) : 
		model(handle),position(pos),scale(scale), orientation(ori),color(col){}
	Model		model;
	vec3		position;
	vec3		scale;
	quaternion	orientation;
	Color		color;
};


struct FrameTexture
{
	u32		texture;
	u32		buffer;
	u32		textureWidth;
	u32		textureHeight;
	int		attachments;
};

enum FrameBuffetAttachment : int
{
	None = 1 << 0,
	ColorAttch = 1 << 1,
	DepthAttch = 1 << 2,
	MultisampleAttch = 1 << 3
};

static inline FrameTexture create_depth_texture(u32 width,u32 height)
{
	FrameTexture ret;
	ret.attachments = FrameBuffetAttachment::DepthAttch;
	ret.textureHeight = height;
	ret.textureWidth = width;
	GLERRCHECK(glGenFramebuffers(1, &ret.buffer));
	GLERRCHECK(glBindFramebuffer(GL_FRAMEBUFFER,ret.buffer));

	GLERRCHECK(glGenTextures(1, &ret.texture));
	GLERRCHECK(glBindTexture(GL_TEXTURE_2D, ret.texture));;
	GLERRCHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
				width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
	GLERRCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLERRCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLERRCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
	GLERRCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLERRCHECK(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));

	GLERRCHECK(glBindFramebuffer(GL_FRAMEBUFFER, ret.buffer));
	GLERRCHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ret.texture, 0));
	GLERRCHECK(glDrawBuffer(GL_NONE));
	GLERRCHECK(glReadBuffer(GL_NONE));
	GLERRCHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));  
	return ret;
}

static inline FrameTexture create_new_frameTexture(u32 width,u32 height,GLenum attachment,int type)
{
	FrameTexture ret;
	ret.textureWidth = width;
	ret.textureHeight = height;
	GLERRCHECK(glGenFramebuffers(1,&ret.buffer));
	GLERRCHECK(glBindFramebuffer(GL_FRAMEBUFFER,ret.buffer));
	GLERRCHECK(glGenTextures(1,&ret.texture));
	GLenum texturetype = 0;
	texturetype = BIT_CHECK(type,FrameBuffetAttachment::MultisampleAttch) ? 
		GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D; 
	GLERRCHECK(glBindTexture(texturetype,ret.texture));
	if(BIT_CHECK(type,FrameBuffetAttachment::MultisampleAttch)) {
		GLERRCHECK(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, width, height, GL_TRUE));
	} else {
		GLERRCHECK(glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,NULL));
	}
	GLERRCHECK(glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR));
	GLERRCHECK(glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR));
	GLERRCHECK(glBindTexture(texturetype,0));
	GLERRCHECK(glFramebufferTexture2D(GL_FRAMEBUFFER,attachment,texturetype,ret.texture,0));
	if(type != FrameBuffetAttachment::DepthAttch && BIT_CHECK(type,FrameBuffetAttachment::DepthAttch))
	{
		u32 rbo = 0;
		GLERRCHECK(glGenRenderbuffers(1,&rbo));
		GLERRCHECK(glBindRenderbuffer(GL_RENDERBUFFER,rbo));
		if(BIT_CHECK(type,FrameBuffetAttachment::MultisampleAttch)) {
			GLERRCHECK(glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4,
						GL_DEPTH24_STENCIL8, width, height));  
		} else {
			GLERRCHECK(glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8,
						ret.textureWidth,ret.textureHeight));
		}
		GLERRCHECK(glBindRenderbuffer(GL_RENDERBUFFER,0));
		GLERRCHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,
					GL_RENDERBUFFER,rbo));
	}
	if(!(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)){
		ABORT_MESSAGE("FAILED TO SET FRAMEBUFFER \n");
	}
	GLERRCHECK(glBindFramebuffer(GL_FRAMEBUFFER,0));
	ret.attachments = type;
	return ret;
}



static inline void set_and_clear_frameTexture(const FrameTexture& frameTex)
{
	GLERRCHECK(glBindFramebuffer(GL_FRAMEBUFFER,frameTex.buffer));
	GLERRCHECK(glViewport(0, 0, frameTex.textureWidth, frameTex.textureHeight));
	glClearColor(0.f,0.f,0.f,1.f);
	if(BIT_CHECK(frameTex.attachments,FrameBuffetAttachment::DepthAttch) && 
			BIT_CHECK(frameTex.attachments,FrameBuffetAttachment::ColorAttch)  ) {
		GLERRCHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}
	else if ( FrameBuffetAttachment::DepthAttch == frameTex.attachments ) {
		GLERRCHECK(glClear(GL_DEPTH_BUFFER_BIT));
	}
	else if ( FrameBuffetAttachment::ColorAttch == frameTex.attachments) {
		GLERRCHECK(glClear(GL_COLOR_BUFFER_BIT));
	}
	else {
		ABORT_MESSAGE("Error with frametexturetype !!");
	}
}

static void inline blit_frameTexture(FrameTexture from,FrameTexture to)
{
	GLERRCHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, from.buffer));
	GLERRCHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to.buffer));
	GLERRCHECK(glBlitFramebuffer(0, 0, from.textureWidth, from.textureHeight, 0, 0,
				to.textureWidth, to.textureHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST)); 

	GLERRCHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
}

static void inline blit_frameTexture(FrameTexture from,u32 to)
{
	GLERRCHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, from.buffer));
	GLERRCHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to));
	GLERRCHECK(glBlitFramebuffer(0, 0, from.textureWidth, from.textureHeight, 0, 0,
				SCREENWIDHT, SCREENHEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST)); 
	GLERRCHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
}
struct SkyRenderer {
	u32 vao;
	u32 texid;
	u32 shader;
};
#define NUM_CASCADES 4
struct ShadowRenderer {
	FrameTexture	cascades[NUM_CASCADES];
	u32				shader;
	mat4			matrixes[NUM_CASCADES];
	float			splitdistances[NUM_CASCADES];
};

struct Renderer {
	u32							modelShader;
	ModelData					models[(int)Model::MaxModels];
	std::vector<RenderData>		renderables;
	SkyRenderer					skyrenderer;
	ShadowRenderer				shadowrenderer;
};

static SkyRenderer get_sky_renderer() 
{
	u32 textureID;
	GLERRCHECK(glGenTextures(1, &textureID));
	GLERRCHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, textureID));
	int width, height, nrChannels;
#define ROOTNAME "textures/"
	const char* texnames[6] = {
		ROOTNAME"right.png",
		ROOTNAME"left.png",
		ROOTNAME"top.png",
		ROOTNAME"down.png",
		ROOTNAME"front.png",
		ROOTNAME"back.png",
	};
	unsigned char *data;  
	for(GLuint i = 0; i < 6; i++) {
		data = stbi_load(texnames[i], &width, &height, &nrChannels, 0);
		ASSERT_MESSAGE(data,"texture not found!");
		GLERRCHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	SkyRenderer rend;
	rend.texid = textureID;
	rend.shader = load_shader("shaders/skyvert.glsl","shaders/skyfrag.glsl");
	rend.vao = skyboxVAO;
	return rend;
}

static ShadowRenderer get_shadow_renderer() 
{
	ShadowRenderer rend;
	for(u32 i = 0; i < NUM_CASCADES;i++) {
		rend.cascades[i] = create_depth_texture(2048,2048);
	}
	rend.shader = load_shader("shaders/shadowvert.glsl", "shaders/shadowfrag.glsl");
	return rend;
}

static Renderer init_renderer() 
{
	Renderer rend;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	LOG("Renderer initing");
	rend.modelShader = load_shader("shaders/model.vert","shaders/model.frag");
	rend.skyrenderer = get_sky_renderer();
	LOG("Shader loaded initing");
	rend.models[(int)Model::Cube] = load_cube();
	// load shadow renderer
	rend.shadowrenderer = get_shadow_renderer();
	glClearColor(0.0f, 0.0f, 0.0f, 1.f);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	LOG("Renderer inited");
	return rend;
}

static inline void render_cube(Renderer* rend,const vec3& pos,const vec3& scale,
		const quaternion& orientation,Color color) 
{
	rend->renderables.emplace_back(Model::Cube,pos,scale,orientation,color);
}
#if 0
static void render_shadows(Renderer* rend,const Camera& cam) 
{
	const float cascadeSplitLambda = 0.550f;
	float nearS = 0.1f;
	float farS  = 100.f;
	float clipRange = farS - nearS;
	float minz = nearS;
	float maxz  = nearS + clipRange;
	float range = maxz - minz;
	float ratio = maxz / minz;

	float cascadeSplits[NUM_CASCADES];
	mat4  lightViews[NUM_CASCADES];
	mat4  shadowOrthos[NUM_CASCADES];
	float splitDepths[NUM_CASCADES];
	// calculate splits
	for(u32 i = 0; i < NUM_CASCADES;i++) {
		float p = (i + 1) / (float)NUM_CASCADES;
		float log = minz * std::pow(ratio,p);
		float uniform = minz + range * p;
		float d = cascadeSplitLambda * (log - uniform) + uniform;
		cascadeSplits[i] = (d - nearS) / clipRange;
	}

	float lastSplitDist = 0;
	for(u32 i = 0; i < NUM_CASCADES;i++) {
		float splitDist = cascadeSplits[i];
		vec3 corners[8] = {
			vec3(-1, 1, -1),
			vec3( 1, 1, -1),
			vec3( 1,-1, -1),
			vec3(-1,-1, -1),
			vec3(-1, 1,  1),
			vec3( 1, 1,  1),
			vec3( 1,-1,  1),
			vec3(-1,-1,  1),
		};
		mat4 tempMat = cam.projection * cam.view;
		mat4 invCam;
		inverse_mat4(&invCam,&tempMat);
		//(*(glm::mat4*)&rend->projection) *
		//(*(glm::mat4*)&rend->view) );
		//
		//project corners to worldspace
		for(u32 i2 = 0; i2 < 8; i2++) {
			vec4 invCorner = invCam * vec4(corners[i2],1.0f);
			corners[i2].x = invCorner.x  /invCorner.w;
			corners[i2].y = invCorner.y  /invCorner.w;
			corners[i2].z = invCorner.z  /invCorner.w;
		}
		for(u32 i2 = 0; i2 < 4; i2++){
			vec3 dist = corners[i2 + 4] - corners[i2];
			corners[i2 + 4] = corners[i2] + (dist * splitDist);
			corners[i2] = corners[i2] + (dist * lastSplitDist);
		}

		//frustum center
		vec3 frustumCenter;
		for(i32 i2 = 0; i2 < 8; i2++) {
			frustumCenter += corners[i2];
		}
		frustumCenter.x /= 8.f;
		frustumCenter.y /= 8.f;
		frustumCenter.z /= 8.f;
		float radius = 0;
		for(i32 i2 = 0; i2 < 8; i2++) {
			float dist = lenght(corners[i2] - frustumCenter);
			radius = max(radius,dist);
		}
		radius = std::ceil(radius * 16.f) / 16.f;
		vec3 maxExtents = vec3(radius);
		vec3 minExtents = get_scaled(maxExtents,-1);

		vec3 lDir = normalized(get_scaled(glight_direction,1.f));

		glm::mat4 temp = glm::lookAt(
				(*(glm::vec3*)&frustumCenter) - 
				(*(glm::vec3*)&lDir) *
				-minExtents.z,
				*(glm::vec3*)&frustumCenter,
				glm::vec3(0,1.f,0));

		mat4 lightViewMatrix = *(mat4*)&temp;

#if 0
		glm::mat4 lightOrthoMatrix_glm = glm::ortho(
				minExtents_glm.x,maxExtents_glm.x,
				minExtents_glm.y,maxExtents_glm.y,
				0.f, maxExtents_glm.z - minExtents_glm.z);

		//*(glm::vec3*)&rend->light.dir);

		mat4 lightViewMatrix;
		create_lookat_mat4(&lightViewMatrix,frustumCenter - lDir * -minExtents.z,frustumCenter,{0,1.f,0});
#endif
		//glm::lookAt(frustumCenter - lDir * -minExtents.z,
		//frustumCenter,
		//glm::vec3(0,1.f,0));
		mat4 lightOrthoMatrix;
		orthomat(&lightOrthoMatrix,
				minExtents.x,maxExtents.x,
				minExtents.y,maxExtents.y,
				0.f, maxExtents.z - minExtents.z);
		splitDepths[i] = (nearS + splitDist * clipRange) * -1.0f;
		lightViews[i] = lightViewMatrix;
		shadowOrthos[i] = lightOrthoMatrix;
		lastSplitDist = cascadeSplits[i];//splitDepths[i];
	}
	for(int i = 0; i < NUM_CASCADES;i++) {
		rend->shadowrenderer.matrixes[i] = shadowOrthos[i] * lightViews[i];
		rend->shadowrenderer.splitdistances[i] = splitDepths[i];
	}
	// now render!
	for(u32 i = 0; i < NUM_CASCADES;i++) {
		set_and_clear_frameTexture(rend->shadowrenderer.cascades[i]);

		GLERRCHECK(glUseProgram(rend->shadowrenderer.shader));
		// bind view and projection matrixes
		GLERRCHECK(glUniformMatrix4fv(VIEWLOC, 1, GL_FALSE, (GLfloat*)&lightViews[i]));
		GLERRCHECK(glUniformMatrix4fv(PROJECTIONLOC, 1, GL_FALSE, (GLfloat*)&shadowOrthos[i]));

		//render all models
		for(RenderData data : rend->renderables)  {
			mat4 model;
			//identify(&model);
			model *= data.orientation;
			translate(&model,data.position);
			scale(&model,data.scale);
			// bind model
			GLERRCHECK(glUniformMatrix4fv(MODELLOC, 1, GL_FALSE, (GLfloat*)&model));
			// render model
			ModelData modeldata = rend->models[(int)data.model];
			glBindVertexArray(modeldata.vao);
			GLERRCHECK(glDrawArrays(GL_TRIANGLES, 0, modeldata.numVerts));
		}
	}
	// restore normal framebuffer
	GLERRCHECK(glBindFramebuffer(GL_FRAMEBUFFER,0));
}
#endif

static inline void render_start(Renderer* rend)
{
	rend->renderables.clear();
}
static void render(Renderer* rend,const Camera& cam) 
{
	// render shadows
#if 1
	glEnable(GL_CULL_FACE); 
	glCullFace(GL_BACK);
	set_and_clear_frameTexture(rend->shadowrenderer.cascades[0]);
	GLERRCHECK(glBindFramebuffer(GL_FRAMEBUFFER,0));
	glCullFace(GL_BACK);
#else

	
	render_shadows(rend,cam);
#endif
	// set up

#if 1
	glViewport(0, 0, SCREENWIDHT, SCREENHEIGHT);
	GLERRCHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	glDepthMask(GL_TRUE);
	//TODO(pate) check cull face order
	//glEnable(GL_CULL_FACE); 
	//glCullFace(GL_FRONT);  
	GLERRCHECK(glUseProgram(rend->modelShader));
	GLERRCHECK(glUniformMatrix4fv(PROJECTIONLOC, 1, GL_FALSE, (GLfloat*)&cam.projection));
	GLERRCHECK(glUniformMatrix4fv(VIEWLOC, 1, GL_FALSE, (GLfloat*)&cam.view));
	// bind shadow data!

	GLERRCHECK(glUniformMatrix4fv(SHADOWMATRIXLOC, 
				NUM_CASCADES, GL_FALSE, (GLfloat*)&rend->shadowrenderer.matrixes));
	GLERRCHECK(glUniform4fv(SHADOWSPLITLOC, 1, (float*)&rend->shadowrenderer.splitdistances));
	// activate shadow textures and use them!

	for(int i = 0; i < NUM_CASCADES; i++) {
		GLERRCHECK(glActiveTexture(GL_TEXTURE0 + SHADOWSAMPLERLOC + i));
		GLERRCHECK(glBindTexture(GL_TEXTURE_2D,
					rend->shadowrenderer.cascades[i].texture));
	}
	for(RenderData data : rend->renderables) 
	{
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
#if 1
	// render skybox
	//glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL); 
	GLERRCHECK(glUseProgram(rend->skyrenderer.shader));
	// bind projection and zero translated view
	mat4 tempview = cam.view;
	tempview.mat[3][0] = 0;
	tempview.mat[3][1] = 0;
	tempview.mat[3][2] = 0;
	tempview.mat[3][3] = 1;
	GLERRCHECK(glUniformMatrix4fv(PROJECTIONLOC, 1, GL_FALSE, (GLfloat*)&cam.projection));
	GLERRCHECK(glUniformMatrix4fv(VIEWLOC, 1, GL_FALSE, (GLfloat*)&tempview));
	// bind texture to first spot
	GLERRCHECK(glActiveTexture(GL_TEXTURE0 + TEXTURELOC));
	GLERRCHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, rend->skyrenderer.texid));
	GLERRCHECK(glBindVertexArray(rend->skyrenderer.vao));
	GLERRCHECK(glDrawArrays(GL_TRIANGLES, 0, 36));
	glDepthFunc(GL_LESS);
#endif 
#endif
}
#endif //MP_RENDERER
