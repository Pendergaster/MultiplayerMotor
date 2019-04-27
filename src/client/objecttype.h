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
	quaternion	orientation;
};

//kun objecti ei ole enään aktiivinen ID menee menee id INACTIIVINEN -> clientti poistaa simulaation

//kun id on erisuuri kuin clientillä spawnataan uusi objecti

//clientti lähettää pelkän input staten
#endif
