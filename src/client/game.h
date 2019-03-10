#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "renderer.h"
#include "utils.h"
#include "math.h"

#define GENERATE_COMPONENT_NAME(name) CONCAT(name,Component) name;
#define GENERATE_TYPE(name) name,
#define GENERATE_VECTOR(name) std::vector<CONCAT(name,Component)> CONCAT(name,Vector);
#define UPDATE_COMPONENT(name) do {std::vector<CONCAT(name,Component)>& temp = game->CONCAT(name,Vector);\
	for(u32 i = 0; i < temp.size(); i++) {\
CONCAT(name,Update)(&temp[i]);\
}} while(0);


#define COMPONENT_TYPES(FUNC)\
	FUNC(Transform)\
	FUNC(Physics)\
	FUNC(Render)\

struct TransformComponent {
	vec3 pos,scale;
	quaternion orientation;
};

// updates transform + physics
struct PhysicsComponent {

};

// pushes box to renderer, color
struct RenderComponent {
	Color color;
};

enum class ComponentType :  u32 {
	COMPONENT_TYPES(GENERATE_TYPE)
		MaxComponent
};

typedef void(*funcptr)(void*); 
#if 0
struct Component {

	Component(ComponentType type,funcptr init_func,funcptr update_func,funcptr dispose_func) :
	type(type) ,init(init_func),update(update_func),dispose(dispose_func){}

	ComponentType	type;
	union {
		COMPONENT_TYPES(GENERATE_COMPONENT_NAME);
	};
	bool			active;
	funcptr			init;
	funcptr			update;
	funcptr			dispose;
};
#endif
struct ComponentTag {
	ComponentType	type;
	u32				index;
};
struct Entity {
	Entity(funcptr init_func,funcptr dispose_func) : init(init_func),dispose(dispose_func){}
	u64		entityid;
	int		numComponents;
	funcptr init;
	funcptr dispose;
};

struct Game {
	std::vector<Entity>		entitys;
	std::vector<Entity*>	entityFreelist;
	COMPONENT_TYPES(GENERATE_VECTOR); // vectors of components
};

struct PhysicsWorldData {
	float gravity;
};

Entity* get_new_entity(Game* game,funcptr init,funcptr dispose) 
{
	Entity* ent = NULL;
	if(game->entityFreelist.empty()) {
		game->entitys.emplace_back(init,dispose);
		ent = &game->entitys.back();
	} else {
		ent = game->entityFreelist.back();
		game->entityFreelist.pop_back();
	}
	if(ent->init){
		ent->init(ent);
	}
	return ent;
}

void PhysicsUpdate(PhysicsComponent* comp) {

}
void TransformUpdate(TransformComponent* comp) {

}
void RenderUpdate(RenderComponent* comp) {

}


void init_game(Game* game) {
}
void update_components(Game* game) {
	COMPONENT_TYPES(UPDATE_COMPONENT)
}
