#include "Frustum.h"

Frustum::Frustum(void)
{
}

Frustum::~Frustum(void)
{
}

void Frustum::ExtractPlanes(glm::mat4& view_matrix, glm::mat4& projection_matrix)
{      
	combined_matrix_ = projection_matrix * view_matrix;

  planes_[LEFT] = glm::column(combined_matrix_, 3) + glm::column(combined_matrix_, 0);
  planes_[RIGHT] = glm::column(combined_matrix_, 3) - glm::column(combined_matrix_, 0);
  planes_[DOWN] = glm::column(combined_matrix_, 3) + glm::column(combined_matrix_, 1);
  planes_[UP] = glm::column(combined_matrix_, 3) - glm::column(combined_matrix_, 1);
  planes_[NEAR] = glm::column(combined_matrix_, 3) + glm::column(combined_matrix_, 2);
  planes_[FAR] = glm::column(combined_matrix_, 3) - glm::column(combined_matrix_, 2);

	// Normalize planes
	for (int i = 0; i < 6; ++i) 
	{	
    //planes_[i] = glm::vec4(glm::normalize(glm::vec3(m_planes[i])), m_planes[i].w);
	}   
}

bool Frustum::IsBoxInFrustum(glm::vec3& min, glm::vec3& max)
{  
	for (int i = 0; i < 6; i++) 
	{
		if (glm::dot(glm::vec3(planes_[i]), glm::vec3(min.x, min.y, min.z)) + planes_[i].w > 0) continue;
    if (glm::dot(glm::vec3(planes_[i]), glm::vec3(max.x, min.y, min.z)) + planes_[i].w > 0) continue;
    if (glm::dot(glm::vec3(planes_[i]), glm::vec3(min.x, max.y, min.z)) + planes_[i].w > 0) continue;
		if (glm::dot(glm::vec3(planes_[i]), glm::vec3(max.x, max.y, min.z)) + planes_[i].w > 0) continue;
		if (glm::dot(glm::vec3(planes_[i]), glm::vec3(min.x, min.y, max.z)) + planes_[i].w > 0) continue;
		if (glm::dot(glm::vec3(planes_[i]), glm::vec3(max.x, min.y, max.z)) + planes_[i].w > 0) continue;
		if (glm::dot(glm::vec3(planes_[i]), glm::vec3(min.x, max.y, max.z)) + planes_[i].w > 0) continue;
		if (glm::dot(glm::vec3(planes_[i]), glm::vec3(max.x, max.y, max.z)) + planes_[i].w > 0) continue;

		// If we get here, it isn't in the frustum because all points are behind of 1 plane
		return false;
	}
     
	// Return a true for the box being inside of the frustum, at least one point is in front of a plane
	return true;
}
