#include "frustum.h"

myfrustum::myfrustum(void)
{
}

myfrustum::~myfrustum(void)
{
}

void myfrustum::extract_planes(glm::mat4& viewMatrix, glm::mat4& projectionMatrix)
{      
  // this calculation doesnt work anymore because matrix is switched from quake
  // or maybe it does because actually we dont care about the order so much.

	glm::mat4 combinedMatrix = viewMatrix * projectionMatrix;

  m_planes[LEFT] = glm::column(combinedMatrix, 3) + glm::column(combinedMatrix, 0);
  m_planes[RIGHT] = glm::column(combinedMatrix, 3) - glm::column(combinedMatrix, 0);
  m_planes[UP] = glm::column(combinedMatrix, 3) + glm::column(combinedMatrix, 1);
  m_planes[DOWN] = glm::column(combinedMatrix, 3) - glm::column(combinedMatrix, 1);
  m_planes[FAR] = glm::column(combinedMatrix, 3) + glm::column(combinedMatrix, 2);
  m_planes[NEAR] = glm::column(combinedMatrix, 3) - glm::column(combinedMatrix, 2);

	// Normalize planes
	for (int i = 0; i < 6; ++i) 
	{	
    m_planes[i] = glm::normalize(m_planes[i]);
	}   
}

// TODO: maybe optimize parameters
bool myfrustum::box_in_frustum(glm::vec3& min, glm::vec3& max)
{  
	for (int i = 0; i < 6; i++) 
	{
		if (glm::dot(glm::vec3(m_planes[i]), glm::vec3(min.x, min.y, min.z)) + m_planes[i].w > 0) continue;
    if (glm::dot(glm::vec3(m_planes[i]), glm::vec3(max.x, min.y, min.z)) + m_planes[i].w > 0) continue;
    if (glm::dot(glm::vec3(m_planes[i]), glm::vec3(min.x, max.y, min.z)) + m_planes[i].w > 0) continue;
		if (glm::dot(glm::vec3(m_planes[i]), glm::vec3(max.x, max.y, min.z)) + m_planes[i].w > 0) continue;
		if (glm::dot(glm::vec3(m_planes[i]), glm::vec3(min.x, min.y, max.z)) + m_planes[i].w > 0) continue;
		if (glm::dot(glm::vec3(m_planes[i]), glm::vec3(max.x, min.y, max.z)) + m_planes[i].w > 0) continue;
		if (glm::dot(glm::vec3(m_planes[i]), glm::vec3(min.x, max.y, max.z)) + m_planes[i].w > 0) continue;
		if (glm::dot(glm::vec3(m_planes[i]), glm::vec3(max.x, max.y, max.z)) + m_planes[i].w > 0) continue;

		// If we get here, it isn't in the frustum because all points are behind of 1 plane
		return false;
	}
     
	// Return a true for the box being inside of the frustum, at least one point is in front of a plane
	return true;
}
