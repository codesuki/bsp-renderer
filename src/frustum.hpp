#ifndef FRUSTUM_HPP_
#define FRUSTUM_HPP_

#include <glm/glm.hpp>

class Frustum
{
public:
	Frustum(void);
	~Frustum(void);

	void ExtractPlanes(glm::mat4& view_matrix, glm::mat4& projection_matrix);
	bool IsBoxInFrustum(glm::vec3& min, glm::vec3& max);

private:
  enum {
    LEFT,
    RIGHT,
    UP,
    DOWN,
    NEAR,
    FAR
  };

  glm::vec4 planes_[6];
  glm::mat4 combined_matrix_;
};

#endif
