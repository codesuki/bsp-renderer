#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "util.h"

class camera
{
public:
	camera(void);
	~camera(void);

	camera(vec3d* position, vec3d* lookat, vec3d* up);

	void strafe(float dir);
	void move(float dir);
	void rotate(int x, int y);
	void rotate(float angle, vec3f axis);

	void updateTime(float time);

	float m_difference;

	vec3d m_position;
	vec3d m_lookat;
	vec3d m_up;
};

#endif /* _CAMERA_H_ */

