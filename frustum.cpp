#include "frustum.h"

myfrustum::myfrustum(void)
{
}

myfrustum::~myfrustum(void)
{
}

void normalize_plane(vec4f& plane)
{
    vec3f& vector = plane.get_sub_vector<3>();
    float len = 1.0 / vector.length();
    plane.x() *= len;
    plane.y() *= len;
    plane.z() *= len;
    plane.w() *= len;
}

void myfrustum::extract_planes(mat4f* viewMatrix, mat4f* projectionMatrix)
{      
	mat4f combinedMatrix = *viewMatrix * *projectionMatrix;

	// Left plane
	m_planes[0][0] = combinedMatrix(4-1,1-1) + combinedMatrix(1-1,1-1);
	m_planes[0][1] = combinedMatrix(4-1,2-1) + combinedMatrix(1-1,2-1);
	m_planes[0][2] = combinedMatrix(4-1,3-1) + combinedMatrix(1-1,3-1);
	m_planes[0][3] = combinedMatrix(4-1,4-1) + combinedMatrix(1-1,4-1);

	// Right plane
	m_planes[1][0] = combinedMatrix(4-1,1-1) - combinedMatrix(1-1,1-1);
	m_planes[1][1] = combinedMatrix(4-1,2-1) - combinedMatrix(1-1,2-1);
	m_planes[1][2] = combinedMatrix(4-1,3-1) - combinedMatrix(1-1,3-1);
	m_planes[1][3] = combinedMatrix(4-1,4-1) - combinedMatrix(1-1,4-1);

	// Top plane
	m_planes[2][0] = combinedMatrix(4-1,1-1) - combinedMatrix(2-1,1-1);
	m_planes[2][1] = combinedMatrix(4-1,2-1) - combinedMatrix(2-1,2-1);
	m_planes[2][2] = combinedMatrix(4-1,3-1) - combinedMatrix(2-1,3-1);
	m_planes[2][3] = combinedMatrix(4-1,4-1) - combinedMatrix(2-1,4-1);

	// Bottom plane
	m_planes[3][0] = combinedMatrix(4-1,1-1) + combinedMatrix(2-1,1-1);
	m_planes[3][1] = combinedMatrix(4-1,2-1) + combinedMatrix(2-1,2-1);
	m_planes[3][2] = combinedMatrix(4-1,3-1) + combinedMatrix(2-1,3-1);
	m_planes[3][3] = combinedMatrix(4-1,4-1) + combinedMatrix(2-1,4-1);

	// Near plane

	m_planes[4][0] = combinedMatrix(4-1,1-1) + combinedMatrix(3-1,1-1);
	m_planes[4][1] = combinedMatrix(4-1,1-1) + combinedMatrix(3-1,2-1);
	m_planes[4][2] = combinedMatrix(4-1,1-1) + combinedMatrix(3-1,3-1);
	m_planes[4][3] = combinedMatrix(4-1,1-1) + combinedMatrix(3-1,4-1);

	// Far plane
	m_planes[5][0] = combinedMatrix(4-1,1-1) - combinedMatrix(3-1,1-1);
	m_planes[5][1] = combinedMatrix(4-1,2-1) - combinedMatrix(3-1,2-1);
	m_planes[5][2] = combinedMatrix(4-1,3-1) - combinedMatrix(3-1,3-1);
	m_planes[5][3] = combinedMatrix(4-1,4-1) - combinedMatrix(3-1,4-1); 

	// Normalize planes
	for (int i = 0; i < 6; ++i) {	
		normalize_plane(m_planes[i]);
	}   
}

// TODO: maybe optimize parameters
bool myfrustum::box_in_frustum(vec3f* min, vec3f* max)
{  
	for (int i = 0; i < 6; i++) {
		if (m_planes[i].distance_to_plane(vec3f(min->x(), min->y(), min->z())) > 0) continue;
		if (m_planes[i].distance_to_plane(vec3f(max->x(), min->y(), min->z())) > 0) continue;
		if (m_planes[i].distance_to_plane(vec3f(min->x(), max->y(), min->z())) > 0) continue;
		if (m_planes[i].distance_to_plane(vec3f(max->x(), max->y(), min->z())) > 0) continue;
		if (m_planes[i].distance_to_plane(vec3f(min->x(), min->y(), max->z())) > 0) continue;
		if (m_planes[i].distance_to_plane(vec3f(max->x(), min->y(), max->z())) > 0) continue;
		if (m_planes[i].distance_to_plane(vec3f(min->x(), max->y(), max->z())) > 0) continue;
		if (m_planes[i].distance_to_plane(vec3f(max->x(), max->y(), max->z())) > 0) continue;

		// If we get here, it isn't in the frustum
		return false;
	}
     
	// Return a true for the box being inside of the frustum
	return true;
}
