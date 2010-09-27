#ifndef _BEZIER_H_
#define _BEZIER_H_

#include "bsp.h"

class bezier {
public:
  bezier();
  ~bezier();

  bsp_vertex calculate_quadratic_bezier(float t, const bsp_vertex* control_vertexes);
  void tessellate(int subdivisions);

  bsp_vertex* m_vertexes;
  bsp_vertex m_control_vertexes[9];
  unsigned int* m_indexes;
};

#endif /* _BEZIER_H_ */

