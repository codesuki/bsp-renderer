#ifndef __FRUSTUM_H__
#define __FRUSTUM_H__

#include "util.h"

class myfrustum
{
public:
	myfrustum(void);
	~myfrustum(void);

	void extract_planes(mat4f* viewMatrix, mat4f* projectionMatrix);
	bool box_in_frustum(vec3f* min, vec3f* max);

	vec4f m_planes[6];
};

#endif
