#ifndef MP_CAMERA
#define MP_CAMERA
//#include <glm/glm.hpp>
#include "math.h"
#include "inputs.h"

struct Camera {
	mat4 projection,view;
	vec3 direction,position;
	float yaw,pitch;
};


static Camera get_camera(vec3 pos,float yaw,float pitch,float fov,float aspect) {
	Camera cam;

	cam.pitch = pitch;
	cam.yaw = yaw;
	cam.direction.x = cosf(deg_to_rad * cam.yaw) * 
		cosf(deg_to_rad * cam.pitch);
	cam.direction.y = sinf(deg_to_rad * cam.pitch);
	cam.direction.z = sinf(deg_to_rad * cam.yaw) * 
		cosf(deg_to_rad * cam.pitch);

	cam.position = pos;

	create_lookat_mat4(&cam.view,pos,pos + cam.direction,{0,1,0});
	perspective(&cam.projection,deg_to_rad*fov,
			aspect, 0.1f, 100.0f);

	return cam;
}

static void update_camera(Camera* cam) {

	vec2 movement = get_mouse_movement();
	float sensitivity = 0.1f;
	scale(&movement,sensitivity);
	cam->pitch -= movement.y;
	cam->yaw += movement.x;
	if(cam->pitch > 89.0f)
		cam->pitch =  89.0f;
	if(cam->pitch < -89.0f)
		cam->pitch = -89.0f;

	cam->direction.x = cosf(deg_to_rad * cam->yaw) * 
		cosf(deg_to_rad * cam->pitch);
	cam->direction.y = sinf(deg_to_rad * cam->pitch);
	cam->direction.z = sinf(deg_to_rad * cam->yaw) * 
		cosf(deg_to_rad * cam->pitch);

	normalize(&cam->direction);

	vec3 up;
	up = cross_product(cam->direction, {0,1,0});
	normalize(&up);
	up = cross_product(up, cam->direction);
	normalize(&up);


	float cameraSpeed = 0.1f;
	if (key_down(Key::KEY_W)) {
		cam->position += cam->direction * cameraSpeed;
	}
	if (key_down(Key::KEY_S)) {
		cam->position -= cam->direction * cameraSpeed;
	}
	if (key_down(Key::KEY_A)) {
		cam->position  -= normalized(cross_product(cam->direction, up)) * cameraSpeed;
	}
	if (key_down(Key::KEY_D)) {
		cam->position  += normalized(cross_product(cam->direction,
					up)) * cameraSpeed;
	}



	create_lookat_mat4(&cam->view,cam->position,
			cam->position + cam->direction,up);
}

#endif //MP_CAMERA
