#include "frustum.h"

myfrustum::myfrustum(void)
{
}

myfrustum::~myfrustum(void)
{
}

void myfrustum::extract_planes(glm::mat4& viewMatrix, glm::mat4& projectionMatrix)
{      
  glm::mat4 p = projectionMatrix * quake2ogl;
	glm::mat4 combinedMatrix = viewMatrix * projectionMatrix;

  m_planes[0] = glm::column(combinedMatrix, 3) + glm::column(combinedMatrix, 0);
  m_planes[1] = glm::column(combinedMatrix, 3) - glm::column(combinedMatrix, 0);
  m_planes[2] = glm::column(combinedMatrix, 3) - glm::column(combinedMatrix, 1);
  m_planes[3] = glm::column(combinedMatrix, 3) + glm::column(combinedMatrix, 1);
  m_planes[4] = glm::column(combinedMatrix, 3) + glm::column(combinedMatrix, 2);
  m_planes[5] = glm::column(combinedMatrix, 3) - glm::column(combinedMatrix, 2);

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

		// If we get here, it isn't in the frustum
		return false;
	}
     
	// Return a true for the box being inside of the frustum
	return true;
}
