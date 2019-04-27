#ifndef COMPTYPE
#define COMPTYPE
#include "utils.h"
#include "math.h"
enum class ObjectType : int {
	Floor,
	FreeSimulation,
	Player,
	Inactive,
};

struct ObjectTracker {
	u32			id;
	ObjectType	type;
	vec3		pos;
	vec3		velocity;
	vec3		angularVelocity;
	quaternion	orientation;
};

const vec3 floor_scale(10,1,10);
const vec3 free_scale(0.5,0.5,0.5);
const vec3 player_scale(1,1,1);
//kun objecti ei ole enään aktiivinen ID menee menee id INACTIIVINEN -> clientti poistaa simulaation

//kun id on erisuuri kuin clientillä spawnataan uusi objecti

//clientti lähettää pelkän input staten
#endif
