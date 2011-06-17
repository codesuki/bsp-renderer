#include "frustum.h"

myfrustum::myfrustum(void)
{
}

myfrustum::~myfrustum(void)
{
}



void myfrustum::extract_planes(mat4f* viewMatrix, mat4f* projectionMatrix)
{      
	mat4f combinedMatrix = *viewMatrix * *projectionMatrix;

	// Left plane
	m_planes[0][0] = combinedMatrix(4,1) + combinedMatrix(1,1);
	m_planes[0][1] = combinedMatrix(4,2) + combinedMatrix(1,2);
	m_planes[0][2] = combinedMatrix(4,3) + combinedMatrix(1,3);
	m_planes[0][3] = combinedMatrix(4,4) + combinedMatrix(1,4);

	// Right plane
	m_planes[1][0] = combinedMatrix(4,1) - combinedMatrix(1,1);
	m_planes[1][1] = combinedMatrix(4,2) - combinedMatrix(1,2);
	m_planes[1][2] = combinedMatrix(4,3) - combinedMatrix(1,3);
	m_planes[1][3] = combinedMatrix(4,4) - combinedMatrix(1,4);

	// Top plane
	m_planes[2][0] = combinedMatrix(4,1) - combinedMatrix(2,1);
	m_planes[2][1] = combinedMatrix(4,2) - combinedMatrix(2,2);
	m_planes[2][2] = combinedMatrix(4,3) - combinedMatrix(2,3);
	m_planes[2][3] = combinedMatrix(4,4) - combinedMatrix(2,4);

	// Bottom plane
	m_planes[3][0] = combinedMatrix(4,1) + combinedMatrix(2,1);
	m_planes[3][1] = combinedMatrix(4,2) + combinedMatrix(2,2);
	m_planes[3][2] = combinedMatrix(4,3) + combinedMatrix(2,3);
	m_planes[3][3] = combinedMatrix(4,4) + combinedMatrix(2,4);

	// Near plane

	m_planes[4][0] = combinedMatrix(3,1);
	m_planes[4][1] = combinedMatrix(3,2);
	m_planes[4][2] = combinedMatrix(3,3);
	m_planes[4][3] = combinedMatrix(3,4);

	// Far plane
	m_planes[5][0] = combinedMatrix(4,1) - combinedMatrix(3,1);
	m_planes[5][1] = combinedMatrix(4,2) - combinedMatrix(3,2);
	m_planes[5][2] = combinedMatrix(4,3) - combinedMatrix(3,3);
	m_planes[5][3] = combinedMatrix(4,4) - combinedMatrix(3,4); 

	// Normalize planes
	for (int i = 0; i < 6; ++i) {	
		m_planes[i].normalize();
	}   

/*
	// Left plane
	m_planes[0][0] = combinedMatrix._14 + combinedMatrix._11;
	m_planes[0][1] = combinedMatrix._24 + combinedMatrix._21;
	m_planes[0][2] = combinedMatrix._34 + combinedMatrix._31;
	m_planes[0][3] = combinedMatrix._44 + combinedMatrix._41;

	// Right plane
	m_planes[1][0] = combinedMatrix._14 - combinedMatrix._11;
	m_planes[1][1] = combinedMatrix._24 - combinedMatrix._21;
	m_planes[1][2] = combinedMatrix._34 - combinedMatrix._31;
	m_planes[1][3] = combinedMatrix._44 - combinedMatrix._41;

	// Top plane
	m_planes[2][0] = combinedMatrix._14 - combinedMatrix._12;
	m_planes[2][1] = combinedMatrix._24 - combinedMatrix._22;
	m_planes[2][2] = combinedMatrix._34 - combinedMatrix._32;
	m_planes[2][3] = combinedMatrix._44 - combinedMatrix._42;

	// Bottom plane
	m_planes[3][0] = combinedMatrix._14 + combinedMatrix._12;
	m_planes[3][1] = combinedMatrix._24 + combinedMatrix._22;
	m_planes[3][2] = combinedMatrix._34 + combinedMatrix._32;
	m_planes[3][3] = combinedMatrix._44 + combinedMatrix._42;

	// Near plane
	m_planes[4][0] = combinedMatrix._13;
	m_planes[4][1] = combinedMatrix._23;
	m_planes[4][2] = combinedMatrix._33;
	m_planes[4][3] = combinedMatrix._43;

	// Far plane
	m_planes[5][0] = combinedMatrix._14 - combinedMatrix._13;
	m_planes[5][1] = combinedMatrix._24 - combinedMatrix._23;
	m_planes[5][2] = combinedMatrix._34 - combinedMatrix._33;
	m_planes[5][3] = combinedMatrix._44 - combinedMatrix._43; 

	// Normalize planes
	for (int i = 0; i < 6; ++i) {	
		D3DXPlaneNormalize(&m_planes[i], &m_planes[i]);
	}      
*/
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
