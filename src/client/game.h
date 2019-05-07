#ifndef MP_GAME
#define MP_GAME
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include "objecttype.h"
#include "renderer.h"
#include "utils.h"
#include "math.h"
#include "inputs.h"
#include "camera.h"
#include <GLFW/glfw3.h>
#define MIKA 1
#if MIKA
#include "Client.h"
#include "cppincludes.h"
#endif
#include <imgui/imgui.h>

#define DEFAULT_DYNAMICARRAY_SIZE 6
static void* get_dynamicArray(const u32 size)
{
	u32 allocSize = size*DEFAULT_DYNAMICARRAY_SIZE ;
	void* ret = malloc(allocSize + sizeof(u32) * 2);
	if(!ret) return 0;
	u32* s = (u32*)ret;
	s[0] = allocSize;
	s[1] = 0;
	return &s[2];
}

static void dispose_dynamicArray(void* arr)
{
	u32* start = (u32*)arr;
	start -= 2;
	free(start);
}

#define dynamic_push_back(ARRAY,VALUE) internal_push_dynamicArray((void**)&ARRAY,VALUE,sizeof(*VALUE))
static inline void internal_push_dynamicArray(void** arr,const void* val,const u32 size)
{
	u32* sizes = (u32*)(*arr);
	sizes -= 2;
	if(sizes[0] < ((sizes[1] + size))) {
		sizes[0] *= 2;
		sizes = (u32*)realloc(sizes,sizes[0] + (sizeof(u32) * 2));
		if(!sizes){
			*arr = 0;
			return;
		}
	}
	u8* start = (u8*)&sizes[2];
	memcpy(&start[sizes[1]],val,size);
	sizes[1] += size;
	*arr = start;
}

#define dynamic_array_size(ARRAY) internal_dynamic_array_size(ARRAY,sizeof(*ARRAY))
static u32 inline internal_dynamic_array_size(void* const arr,u32 size) {
	u32* sizes = (u32*)arr;
	sizes -= 2;
	return sizes[1] / size;
}

struct ThightArray {
	u8*		start;
	u32		stride;
	u32		lastindex;
	u32		size;
};

static u32 init_thightarray(ThightArray* arr,u32 stride,void* mem,u32 size) 
{
	memset(arr,0,sizeof *arr);
	arr->size = size;
	arr->start = (u8*)mem;
	arr->stride = stride;
	return size * stride;
}

static inline void* access_index(const ThightArray* arr,u32 index)
{
	ASSERT_MESSAGE(index <= arr->lastindex,"MEMORY COLLISION ERROR ON THIGHTARRAY");
	return (void*)&(arr->start[arr->stride * index]);
}

static void insert_to_tight_array(ThightArray* arr,void* put) 
{
	ASSERT_MESSAGE(arr->lastindex < arr->size,"THIGHTARRAY MEM ENDED!"); 
	void* insert = access_index(arr,arr->lastindex);
	memcpy(insert,put,arr->stride);
	arr->lastindex += 1;
}

static void* get_back_from_tight_array(ThightArray* arr) 
{
	void* ret = (void*)&arr->start[(arr->lastindex - 1) * arr->stride];
	return ret;
}

static void delete_from_tight_array(ThightArray* arr,u32 index) 
{
	void* back = get_back_from_tight_array(arr);
	void* indexptr = access_index(arr,index);
	if(index != arr->lastindex) {
		memcpy(indexptr,back,arr->stride);
	}
	arr->lastindex -= 1;
}

struct Pool {
	u8*			pool;
	u32			stride;
	u32			lastIndex;
	ThightArray	freeArr;
};

static u32 init_pool(Pool* pool,u32 stride,void* mem,u32 size)
{
	memset(pool,0,sizeof *pool);
	pool->stride = stride;
	pool->pool = (u8*)mem;
	u32 usedMem = size * stride;
	usedMem += init_thightarray(&pool->freeArr,sizeof(void*),((u8*)mem) + usedMem,size);
	return usedMem;
}

static void* get_from_pool(Pool* pool) 
{
	u8* ret = NULL;
	if(pool->freeArr.lastindex == 0) {
		// no free components
		ASSERT_MESSAGE(pool->lastIndex <= pool->freeArr.size,"MEMORY ENDED FROM POOL");
		ret = &pool->pool[pool->lastIndex * pool->stride];
		pool->lastIndex++;
	} else {
		// free components
		ret = (u8*)get_back_from_tight_array(&pool->freeArr);
		pool->freeArr.lastindex -= 1;
	}
	return ret;
}

static void return_to_pool(Pool* pool,void* ret) 
{
	insert_to_tight_array(&pool->freeArr,ret);
}



#define IF_PREPROCESSOR_true(...) ; ##__VA_ARGS__
#define IF_PREPROCESSOR_false(...)
#define IF_PREPROCESSOR(cond,...) IF_PREPROCESSOR_##cond(##__VA_ARGS__)

#define GENERATE_COMPONENT_NAME(name,...) CONCAT(name,Component) name;
#define GENERATE_TYPE(name,...) name,
#define GENERATE_SIZES(name,num,...) num,
#define GENERATE_COMPONENT_SIZES(name,...) sizeof(CONCAT(name,Component)),
#define GENERATE_UPDATE_BOOLS(name,num,update) update,
#define GENERATE_POINTERALIASES(name,num,...) game->CONCAT(name,Pool) = (CONCAT(name,Component)*)(componentmem + usedmem);usedmem += sizeof(CONCAT(name,Component)) * num;
#define GENERATE_COMPONENTPOOL(name,...) CONCAT(name,Component)* CONCAT(name,Pool);u32 CONCAT(name,Index);
#if 0
#define GENERATE_GET_COMPONENT_FUNCTION(name,...) static CONCAT(name,Component)* CONCAT(CONCAT(Get,name),COMPONENT)(Game* game){\
	CONCAT(name,Component)* ret = NULL;\
	if(game->freeComponents[name].lastindex != 0) {\
		ret = (CONCAT(name,Component)*)get_back_from_tight_array(&game->freeComponents[name]);\
		game->freeComponents[name].lastindex -= 1;\
	} else {\
		if(game->CONCAT(name,Index) >= ENTITY_MAX_SIZES[name]) ABORT_MESSAGE("TOO MANY "#name" COMPONENTS!");\
		ret = &game->CONCAT(name,Pool)[game->CONCAT(name,Index)];\
		game->CONCAT(name,Index)++;\
	}\
	memset(ret,0,sizeof *ret);\
	ret->header.type = name;\
	return ret;\
}
#endif

#define UPDATE_COMPONENT(name,num,update) do {\
	IF_PREPROCESSOR(update,\
			CONCAT(name,Component)** temp = (CONCAT(name,Component)**)(game->updateArrays[name].start);\
			for(u32 i = 0; i <  game->updateArrays[name].lastindex; i++) {\
			if(temp[i]->header.type != numeric_max_u32){\
			CONCAT(name,Update)(temp[i],game);\
			} else {\
			return_to_pool(&game->componentPools[name],temp[i]);\
			delete_from_tight_array(&game->updateArrays[name],i);\
			i--;\
			}\
			});\
} while(0);

#define UPDATEFUNC(name) void CONCAT(name,Update)(CONCAT(name,Component)* comp,Game* game)
#define COMPONENTINIT(name,...) void CONCAT(name,Init)(CONCAT(name,Component)* comp,Entity* ent,__VA_ARGS__)
struct Entity;
#define DECLARECOMPONENT(name,...) struct CONCAT(name,Component){ComponentHeader header;__VA_ARGS__}

struct ComponentHeader {
	u32					type;
	ComponentHeader*	next;
};

#define MAXENTITIES 10000
#define COMPONENT_TYPES(FUNC)\
	FUNC(Transform,MAXENTITIES,false)\
	FUNC(NetWorkSync,1,true)\
	FUNC(Render,MAXENTITIES,true)\
	FUNC(PlayerCamera,1,true)\


DECLARECOMPONENT(Transform,
		vec3 pos,scale;
		quaternion orientation;
		);

DECLARECOMPONENT(PlayerCamera,
		TransformComponent* playerTrans;
		);


struct ClientObjectTracker {
	ObjectTracker			track;
	//double					time;
};

struct InterpolationData {
	ClientObjectTracker		last;
	ClientObjectTracker		first;
	Entity*					ent;
	TransformComponent*		trans;
};

DECLARECOMPONENT(NetWorkSync,
		InterpolationData*		objs;
		double					currentTime;
		double					lastTime;
		double					targetTime;
		);

// pushes box to renderer, color
DECLARECOMPONENT(Render,
		Color				color;
		TransformComponent* transform;
		);

enum ComponentType : u32 {
	COMPONENT_TYPES(GENERATE_TYPE)
		MaxComponent
};
static u32 ENTITY_MAX_SIZES[] = {
	COMPONENT_TYPES(GENERATE_SIZES)
};
static u32 ENTITY_SIZES[] = {
	COMPONENT_TYPES(GENERATE_COMPONENT_SIZES)
};
static bool ENTITY_UPDATES[] = {
	COMPONENT_TYPES(GENERATE_UPDATE_BOOLS)
};

typedef void(*funcptr)(void*); 
typedef u64 EntityId; // id + tag
struct EntityTrack {
	u32 index;
};
struct Entity {
	Entity(funcptr dispose_func) : entityid(0),entitytag(0),componentFlags(0),dispose(dispose_func),components(0){}
	EntityId			entityid;
	i32					entitytag;
	u32					componentFlags;
	funcptr				dispose;
	ComponentHeader*	components;
};

struct Game {
	Pool						entities;
	Renderer					renderer;	
	ThightArray					updateArrays[MaxComponent];
	Pool						componentPools[MaxComponent];
	Input						inputs;
	// mikan connectio luokka
	Client						connection;
	Camera						camera;
};

Entity* get_new_entity(Game* game,funcptr dispose,ComponentHeader** components,const u32 numComponents) 
{
	Entity* ent = (Entity*)get_from_pool(&game->entities);
	// empty
	ent->dispose = dispose;
	ent->components = components[0];
	for(u32 i = 0; i < numComponents;i++) {
		components[i]->next = i < (numComponents-1) ? components[i+1] : NULL;
	}
	return ent;
}

//COMPONENT_TYPES(GENERATE_GET_COMPONENT_FUNCTION);

void* get_component(Game* game,ComponentType component) 
{
	ComponentHeader* temp =	(ComponentHeader*)get_from_pool(&game->componentPools[component]);
	temp->type = component;
	if(ENTITY_UPDATES[component]) {
		insert_to_tight_array(&game->updateArrays[component],&temp);
	}
	return temp;
}

void* get_component_from_entity(Entity* entity, ComponentType component) 
{
	ComponentHeader* comp = entity->components;
	while(comp) {
		if(comp->type == component) {
			return comp;
		}
		comp = comp->next;
	}
	return NULL;
}

void dispose_entity(Game* game,Entity* ent) 
{
	ComponentHeader* temp = ent->components;
	while(temp) {
		// if does not update dispose now
		if(!ENTITY_UPDATES[temp->type]) {
			return_to_pool(&game->componentPools[temp->type],temp);
		}
		temp->type = numeric_max_u32;
		temp = temp->next;
	}
	return_to_pool(&game->entities,ent);
	ent->entitytag++;
}

COMPONENTINIT(Transform,vec3 pos,vec3 scale,quaternion orientation) {
	(void)ent;
	comp->pos = pos;
	comp->scale = scale;
	comp->orientation = orientation;
}

COMPONENTINIT(Render,Color color) {
	comp->color = color;
	comp->transform = (TransformComponent*)get_component_from_entity(ent,Transform);
	ASSERT_MESSAGE(comp->transform,"failed to find transform!!");
}

COMPONENTINIT(NetWorkSync) {
	(void)ent;
	comp->objs = (InterpolationData*)get_dynamicArray(sizeof(InterpolationData));
	comp->currentTime = glfwGetTime();
}

COMPONENTINIT(PlayerCamera,TransformComponent* playerTrans) {
	(void)ent;
	comp->playerTrans = playerTrans;
}

UPDATEFUNC(Render) {
	render_cube(&game->renderer,comp->transform->pos,
			comp->transform->scale,comp->transform->orientation,comp->color);
}

UPDATEFUNC(PlayerCamera) {
	(void)comp;(void)game;
	vec3 targetPosition;
	vec3 rot = quat_to_euler(comp->playerTrans->orientation);
	ImGui::Text("player rotaton: %f %f %f",rot.x,rot.y,rot.z);
}

Entity* get_player_object(Game* game,int cubeID,const std::vector<PlayerData>& data,
		vec3 pos,vec3 scale,quaternion orientation);
Entity* get_floor_object(Game* game,vec3 pos,vec3 scale,quaternion orientation);
Entity* get_freesimulation_object(Game* game,vec3 pos,vec3 scale,quaternion orientation);

InterpolationData spawn_network_object(Game* game,const ObjectTracker& track,int id) {
	LOG("Spewning new object!");
	InterpolationData ret;
	ret.first.track = track;
	ret.last.track = track;
	switch(track.type) {
		case ObjectType::Floor: {
									Entity* temp = get_floor_object(game,track.pos,floor_scale,
											track.orientation);
									ret.ent = temp;
									ret.trans = (TransformComponent*)get_component_from_entity(temp,Transform);
									ASSERT_MESSAGE(ret.trans,"FAILED TO FIND TRANSFORM");
								} break;
		case ObjectType::FreeSimulation: {
											 Entity* temp = get_freesimulation_object(game,track.pos,
													 free_scale,track.orientation);
											 ret.ent = temp;
											 ret.trans = (TransformComponent*)get_component_from_entity(temp,Transform);
											 ASSERT_MESSAGE(ret.trans,"FAILED TO FIND TRANSFORM");
										 } break;
		case ObjectType::Player: {
									 const std::vector<PlayerData>& pldata = game->connection.Players;
									 Entity* temp = get_player_object(game,id,pldata,track.pos,
											 player_scale,track.orientation);
									 ret.ent = temp;
									 ret.trans = (TransformComponent*)get_component_from_entity(temp,Transform);
									 ASSERT_MESSAGE(ret.trans,"FAILED TO FIND TRANSFORM");

								 } break;
		default : ABORT_MESSAGE("unknown object spawning!");
	}
	return ret;
}

UPDATEFUNC(NetWorkSync) {
	comp->currentTime = glfwGetTime();
	if(game->connection.isNewData) {
		comp->lastTime = comp->targetTime;
		comp->targetTime = comp->currentTime;
		const std::vector<ObjectTracker>& serverobjs = game->connection.Objects;
		for(size_t i = 0; i < serverobjs.size();i++) {
			if(i >= dynamic_array_size(comp->objs)) {
				// spawn if more objecs have came		
				InterpolationData temp = spawn_network_object(game,serverobjs[i],i);
				//internal_push_dynamicArray((void**)&comp->objs,&temp,sizeof(*comp->objs));
				dynamic_push_back(comp->objs,&temp);
				//comp->objs.push_back(temp);
			} else if(serverobjs[i].type != comp->objs[i].first.track.type) {
				// (de)spawn if type is different
				if(serverobjs[i].type != ObjectType::Inactive) {
					InterpolationData temp = spawn_network_object(game,serverobjs[i],i);
					comp->objs[i] = temp;
				} else {
					dispose_entity(game,comp->objs[i].ent);
					comp->objs[i].first.track.type =  ObjectType::Inactive;
					comp->objs[i].last.track.type =  ObjectType::Inactive;
				}
			} else if(comp->objs[i].first.track.type != ObjectType::Inactive){
				//sync objects
				comp->objs[i].first = comp->objs[i].last;
				//comp->objs[i].first.track.pos = comp->objs[i].trans->pos;
				comp->objs[i].last.track = serverobjs[i];
			}
		}
	} 
	size_t size = dynamic_array_size(comp->objs);
	for(InterpolationData* data = comp->objs; data != comp->objs + size; data++) { 
		if(data->first.track.type == ObjectType::Inactive) continue;
		vec3 target = data->last.track.pos - data->first.track.pos;
		target = data->first.track.pos + target;
		double timeWindow = comp->targetTime - comp->lastTime;
		double current = comp->currentTime - comp->targetTime;
		double delta = current / timeWindow;
		data->trans->pos = vec_lerp(data->first.track.pos,data->last.track.pos,delta);
		data->trans->orientation = interpolate_q(data->first.track.orientation,
				data->last.track.orientation,delta);
	}
}

static char name[64] = ""; 
bool update_components(Game* game) {
	//ImGui::ShowDemoWindow();
	ImGui::Begin("Supa kame");
	defer{ImGui::End();};
	static bool	loggedIn = false;
	if(!loggedIn) {
		static bool	triedToLog = false;
		static char buf1[12] = ""; 
		static int port = 0;
		if(!triedToLog) {
			static bool fileLoaded = false;
			if(!fileLoaded)  {
				if(does_file_exist("connection.info")) {
					size_t size = 0;
					char* data = load_file("connection.info",&size);
					if(size < 64){
						sscanf(data,"%s %d %s",buf1,&port,name);
					} 
					free(data);
					fileLoaded = true;
				}
			}
			ImGui::InputText("name", name, 12);
			ImGui::InputText("address", buf1, 64);
			ImGui::InputInt("port", &port, 1);
			if (ImGui::Button("Log in")) {
				// 60000
				game->connection.Init(buf1,abs(port), name);
				game->connection.OpenConnection();
				triedToLog = true;
				FILE* f = fopen("connection.info","w");
				if(f) fprintf(f,"%s %d %s",buf1,port,name);
				fclose(f);
			}
		} else {
			if (LOGIN_ACCEPTED == game->connection.Update()) {
				loggedIn = true;
				disable_cursor();
			} 		
			static double timer = 4.0;

			timer -= 1.0 / 60.0;
			if(timer < 0 ) return 0;
		}

	} else {
		game->connection.Update();


		COMPONENT_TYPES(UPDATE_COMPONENT)
	}
	return 1;
}


Entity* get_networksynch_object(Game* game);
void init_game(Game* game) 
{
	memset(game,0,(sizeof *game) - sizeof(Client));
	u32 totalsize = sizeof(Entity) * MAXENTITIES + sizeof(Entity*) * MAXENTITIES;
	for(u32 i = 0; i < MaxComponent;i++) {
		if(ENTITY_UPDATES[i]) {
			totalsize += ENTITY_MAX_SIZES[i] * sizeof(void*);
		}
		totalsize += ENTITY_MAX_SIZES[i] * ENTITY_SIZES[i];
		totalsize += ENTITY_MAX_SIZES[i] * sizeof(void*);
	}
	LOG("allocating %d memory for components",totalsize);
	u8* gamememory = (u8*)malloc(totalsize);
	ASSERT_MESSAGE(gamememory,"too many components!");
	u32 usedmem = 0;
	usedmem += init_pool(&game->entities,sizeof(Entity),gamememory + usedmem,MAXENTITIES);;
	for(i32 i = 0; i < (i32)MaxComponent;i++) {
		usedmem += init_pool(&game->componentPools[i],ENTITY_SIZES[i],gamememory + usedmem,ENTITY_MAX_SIZES[i]);
	}
	//COMPONENT_TYPES(GENERATE_POINTERALIASES);
	// init update vectors
	for(i32 i = 0; i < (i32)MaxComponent;i++) {
		if(ENTITY_UPDATES[i]) {
			usedmem += init_thightarray(&game->updateArrays[i],sizeof(void*),gamememory + usedmem,ENTITY_MAX_SIZES[i]);
		}
	}

	// INIT GAMES START COMPONENTS
	//Entity* player = get_player_object(game);
	//Entity* floor = get_floor_object(game,{0,0,0},{5,1,5});
	Entity* net = get_networksynch_object(game);
	//(void)player;(void)floor;(void)net;
	game->camera = get_camera(
			{0.f,20.f,-6.f},
			0,-90,
			90.f,	
			(float)SCREENWIDHT / (float)SCREENHEIGHT
			);

}

static Color colors[4] = {
	{255,100,100,255},
	{100,255,100,255},
	{100,100,255,255},
	{100,100,100,255}};

Entity* get_player_object(Game* game,int cubeID,const std::vector<PlayerData>& data,
		vec3 pos,vec3 scale,quaternion orientation) 
{
	(void)colors;
	(void)data;
	RenderComponent* rend = (RenderComponent*)get_component(game,Render);
	TransformComponent* tran = (TransformComponent*)get_component(game,Transform);
	bool playerFound = false;
	for(const PlayerData& player : data) {
		if(player.kuutioId == cubeID && !strcmp(player.name,name)) {
			playerFound = true;
		}
	}
	Entity* ent = NULL;
	if(playerFound) {
		PlayerCameraComponent* cam = (PlayerCameraComponent*)get_component(game,PlayerCamera);
		ComponentHeader* components[] = {(ComponentHeader*)rend,(ComponentHeader*)tran,(ComponentHeader*)cam};
		ent = get_new_entity(game,NULL,components,ARRAY_SIZE(components));
	} else {
		ComponentHeader* components[] = {(ComponentHeader*)rend,(ComponentHeader*)tran};
		ent = get_new_entity(game,NULL,components,ARRAY_SIZE(components));
	}
	RenderInit(rend,ent,{255,0,0,255});
	TransformInit(tran,ent,pos,scale,orientation);
	// COMPONENTINIT(Transform,vec3 pos,vec3 scale,quaternion orientation) {
	return ent;
}

Entity* get_floor_object(Game* game,vec3 pos,vec3 scale,quaternion orientation) 
{
	RenderComponent* rend = (RenderComponent*)get_component(game,Render);
	TransformComponent* tran = (TransformComponent*)get_component(game,Transform);
	ComponentHeader* components[] = {(ComponentHeader*)rend,(ComponentHeader*)tran};
	Entity* ent = get_new_entity(game,NULL,components,ARRAY_SIZE(components));
	if(pos.x < 0 && pos.z == 0) 
		RenderInit(rend,ent,colors[0]);
	else if(pos.x == 0 && pos.z < 0) 
		RenderInit(rend,ent,colors[1]);
	else if(pos.x > 0 && pos.z == 0) 
		RenderInit(rend,ent,colors[2]);
	else if(pos.x == 0 && pos.z > 0) 
		RenderInit(rend,ent,colors[3]);
	else RenderInit(rend,ent,{255,255,255,255});

	TransformInit(tran,ent,pos,scale,orientation);
	// COMPONENTINIT(Transform,vec3 pos,vec3 scale,quaternion orientation) {

	return ent;
}

Entity* get_freesimulation_object(Game* game,vec3 pos,vec3 scale,quaternion orientation)
{
	RenderComponent* rend = (RenderComponent*)get_component(game,Render);
	TransformComponent* tran = (TransformComponent*)get_component(game,Transform);
	ComponentHeader* components[] = {(ComponentHeader*)rend,(ComponentHeader*)tran};
	Entity* ent = get_new_entity(game,NULL,components,ARRAY_SIZE(components));
	RenderInit(rend,ent,{0,255,0,1});
	TransformInit(tran,ent,pos,scale,orientation);
	// COMPONENTINIT(Transform,vec3 pos,vec3 scale,quaternion orientation) {
	return ent;
}

Entity* get_networksynch_object(Game* game) {
	NetWorkSyncComponent* net = (NetWorkSyncComponent*)get_component(game,NetWorkSync);
	ComponentHeader* components[] = {(ComponentHeader*)net};
	Entity* ent = get_new_entity(game,NULL,components,ARRAY_SIZE(components));
	NetWorkSyncInit(net,ent);
	return ent;
}
#endif
