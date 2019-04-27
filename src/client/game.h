#ifndef MP_GAME
#define MP_GAME
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "renderer.h"
#include "utils.h"
#include "math.h"
#include "inputs.h"

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
	FUNC(Physics,MAXENTITIES,true)\
	FUNC(Render,MAXENTITIES,true)


DECLARECOMPONENT(Transform,
		vec3 pos,scale;
		quaternion orientation;
		);


// updates transform + physics
DECLARECOMPONENT(Physics);

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
	Pool					entities;
	Renderer				renderer;	
	ThightArray				updateArrays[MaxComponent];
	Pool					componentPools[MaxComponent];
	Input					inputs;
	// mikan connectio luokka
};

struct PhysicsWorldData {
	vec3 gravity;
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

UPDATEFUNC(Physics) {
	(void)comp;(void)game;
}

COMPONENTINIT(Transform,vec3 pos,vec3 scale,quaternion orientation) {
	(void)ent;
	comp->pos = pos;
	comp->scale = scale;
	comp->orientation = orientation;
}

COMPONENTINIT(Render) {
	comp->transform = (TransformComponent*)get_component_from_entity(ent,Transform);
	assert(comp->transform);
}
UPDATEFUNC(Render) {
	render_cube(&game->renderer,comp->transform->pos,
				comp->transform->scale,comp->transform->orientation,comp->color);
}

void update_components(Game* game) {
#if 0
	do { ;
		CONCAT(Physics,Component)** temp = (CONCAT(Physics,Component)**)(game->updateArrays[Physics].start); 
		for(u32 i = 0; i < game->updateArrays[Physics].lastindex; i++) 
		{ if(temp[i]->header.type != numeric_max_u32) { 
			CONCAT(Physics,Update)(temp[i],game); 
		} else { 
			return_to_pool(&game->componentPools[Physics],temp[i]); 
			delete_from_tight_array(&game->updateArrays[Physics],i); 
			i--; 
		} 
		
		};
	} while(0); 

	do { ;
		CONCAT(Render,Component)** temp = (CONCAT(Render,Component)**)(game->updateArrays[Render].start); 
		for(u32 i = 0; i < game->updateArrays[Render].lastindex; i++) { 
			if(temp[i]->header.type != numeric_max_u32){ CONCAT(Render,Update)(temp[i],game); 
		} else { 
			return_to_pool(&game->componentPools[Render],temp[i]); 
			delete_from_tight_array(&game->updateArrays[Render],i); 
			i--; 
		} 
		};
	} while(0);

#else
	COMPONENT_TYPES(UPDATE_COMPONENT)
#endif
}


Entity* get_player_component(Game* game);
Entity* get_floor_component(Game* game,vec3 pos,vec3 scale);
void init_game(Game* game) 
{
	memset(game,0,sizeof *game);
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
	Entity* player = get_player_component(game);
	Entity* floor = get_floor_component(game,{0,0,0},{5,1,5});
	(void)player;
	(void)floor;

	//Server
	int id = 0;
	id++;


	[1 + type {pos,orientation}]
	[7 + type {pos,orientation}]
	[3 + type {pos,orientation}]
	[4 + type {pos,orientation}]
	[5 + type {pos,orientation}]
	[6 + type {pos,orientation}]
	//spawn object -> 
	//Client
	
	[1 + type ][7 + type ][3 + type ][4 + type ][5 + type ][6 + type ]

	 {
		vec of objects : 
		id			   : 
	 }


	Client lähettää takas pelkän input staten
}

Entity* get_player_component(Game* game) 
{
	RenderComponent* rend = (RenderComponent*)get_component(game,Render);
	TransformComponent* tran = (TransformComponent*)get_component(game,Transform);
	ComponentHeader* components[] = {(ComponentHeader*)rend,(ComponentHeader*)tran};
	Entity* ent = get_new_entity(game,NULL,components,ARRAY_SIZE(components));
	RenderInit(rend,ent);
	TransformInit(tran,ent,{0,2,0},{1,1,1},{0,0,0,1});
	// COMPONENTINIT(Transform,vec3 pos,vec3 scale,quaternion orientation) {
	return ent;
}
Entity* get_floor_component(Game* game,vec3 pos,vec3 scale) 
{
	RenderComponent* rend = (RenderComponent*)get_component(game,Render);
	TransformComponent* tran = (TransformComponent*)get_component(game,Transform);
	ComponentHeader* components[] = {(ComponentHeader*)rend,(ComponentHeader*)tran};
	Entity* ent = get_new_entity(game,NULL,components,ARRAY_SIZE(components));
	RenderInit(rend,ent);
	TransformInit(tran,ent,pos,scale,{0,0,0,1});
	// COMPONENTINIT(Transform,vec3 pos,vec3 scale,quaternion orientation) {
	return ent;
}

#endif
