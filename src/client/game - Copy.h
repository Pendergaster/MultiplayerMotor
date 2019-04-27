#ifndef MP_GAME
#define MP_GAME
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "renderer.h"
#include "utils.h"
#include "math.h"

struct UpdateArray {
	void*	start;
	u32		stride;
};

void init_update_array(UpdateArray) {

}

#define IF_PREPROCESSOR_true(...) do{##__VA_ARGS__}while(0)
#define IF_PREPROCESSOR_false(...)
#define IF_PREPROCESSOR(cond,...) IF_PREPROCESSOR_##cond(##__VA_ARGS__)

#define GENERATE_COMPONENT_NAME(name,...) CONCAT(name,Component) name;
#define GENERATE_TYPE(name,...) name,
#define GENERATE_SIZES(name,num,...) num,
#define GENERATE_UPDATE_BOOLS(name,num,update) update,
#define GENERATE_VECTOR(name,...) std::vector<CONCAT(name,Component)*> CONCAT(name,UpdateVector); std::vector<CONCAT(name,Component)*> CONCAT(name,FreeVector);
#define GENERATE_TOTALSIZE(name,num,...) sizeof(CONCAT(name,Component)) * num +  
#define GENERATE_POINTERALIASES(name,num,...) game->CONCAT(name,Pool) = (CONCAT(name,Component)*)componentmem;usedmem += sizeof(CONCAT(name,Component)) * num;
#define GENERATE_COMPONENTPOOL(name,...) CONCAT(name,Component)* CONCAT(name,Pool);u32 CONCAT(name,Index);
#define GENERATE_GET_COMPONENT_FUNCTION(name,...) static CONCAT(name,Component)* CONCAT(CONCAT(Get,name),COMPONENT)(Game* game){\
	CONCAT(name,Component)* ret = NULL;\
	if(game->CONCAT(name,FreeVector).empty()) {\
		ret = game->CONCAT(name,FreeVector).back();\
		game->CONCAT(name,FreeVector).pop_back();\
	} else {\
		if(game->CONCAT(name,Index) >= ENTITY_MAX_SIZES[name]) ABORT_MESSAGE("TOO MANY "#name" COMPONENTS!");\
		ret = &game->CONCAT(name,Pool)[game->CONCAT(name,Index)];\
		game->CONCAT(name,Index)++;\
	}\
	memset(ret,0,sizeof *ret);\
	ret->header.type = name;\
	return ret;\
}

#define UPDATE_COMPONENT(name,num,update) do {\
	IF_PREPROCESSOR(update,\
			std::vector<CONCAT(name,Component)*>& temp = game->CONCAT(name,UpdateVector);\
			for(u32 i = 0; i < temp.size(); i++) {\
			if(temp[i]->header.type != numeric_max_u32){\
			CONCAT(name,Update)(temp[i],game);\
			} else {\
			game->CONCAT(name,FreeVector).push_back(temp[i]);\
			temp[i] = temp.back();\
			temp.pop_back();\
			i--;\
			}\
			});\
} while(0);

#define UPDATEFUNC(name) void CONCAT(name,Update)(CONCAT(name,Component)* comp,Game* game)
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
	FUNC(Render,MAXENTITIES,true)\

DECLARECOMPONENT(Transform,
		vec3 pos,scale;
		quaternion orientation;
		);

// updates transform + physics
DECLARECOMPONENT(Physics);

// pushes box to renderer, color
DECLARECOMPONENT(Render,
		Color color;
		);

enum ComponentType : u32 {
	COMPONENT_TYPES(GENERATE_TYPE)
		MaxComponent
};
static u32 ENTITY_MAX_SIZES[] = {
	COMPONENT_TYPES(GENERATE_SIZES)
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
	std::vector<Entity>		entitys;
	std::vector<Entity*>	entityFreelist;
	COMPONENT_TYPES(GENERATE_VECTOR); // vectors of components for updating
	COMPONENT_TYPES(GENERATE_COMPONENTPOOL); // vectors of components
};

struct PhysicsWorldData {
	vec3 gravity;
};

Entity* get_new_entity(Game* game,funcptr dispose,ComponentHeader** components,u32 numComponents) 
{
	Entity* ent = NULL;
	if(game->entityFreelist.empty()) {
		game->entitys.emplace_back(dispose);
		ent = &game->entitys.back();
	} else {
		ent = game->entityFreelist.back();
		game->entityFreelist.pop_back();
	}
	ent->components = components[0];
	for(u32 i = 0; i < numComponents;i++) {
		components[i]->next = i < (numComponents-1) ? components[i+1] : NULL;
	}
	return ent;
}

void init_game(Game* game) 
{
	memset(game,0,sizeof *game);
	u32 totalsize = COMPONENT_TYPES(GENERATE_TOTALSIZE) 0;
	LOG("allocating %d memoty for components",totalsize);
	u8* componentmem = (u8*)malloc(totalsize);
	u32 usedmem = 0;
	COMPONENT_TYPES(GENERATE_POINTERALIASES);
}

	template<typename T>
T* internal_add_component(std::vector<T>* vec,Entity* ent,ComponentType type) 
{
	vec->push_back(T());
	BIT_SET(ent->componentFlags,1 << (int)type);
	return &vec->back();
}

COMPONENT_TYPES(GENERATE_GET_COMPONENT_FUNCTION)

	void dispose_entity(Game* game,Entity* ent) {
		ComponentHeader* temp = ent->components;
		while(temp) {
			temp->type = numeric_max_u32;
			temp = temp->next;
		}
		game->entityFreelist.push_back(ent);
		ent->entitytag++;
	}

UPDATEFUNC(Physics) {
	(void)comp;(void)game;
}

UPDATEFUNC(Render) {
	(void)comp;(void)game;
}

void update_components(Game* game) {
	COMPONENT_TYPES(UPDATE_COMPONENT)
}
#endif
