#ifndef __FRUSTUM_H__
#define __FRUSTUM_H__

#include "util.h"

class myfrustum
{
public:
	myfrustum(void);
	~myfrustum(void);

	void extract_planes(glm::mat4& viewMatrix, glm::mat4& projectionMatrix);
	bool box_in_frustum(glm::vec3& min, glm::vec3& max);

	glm::vec4 m_planes[6];
};

#endif
