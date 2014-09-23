#ifndef BEZIER_HPP_
#define BEZIER_HPP_

#include "bsp.hpp"

class bezier {
public:
  bezier();
  ~bezier();

  bsp_vertex calculate_quadratic_bezier(float t, const bsp_vertex* control_vertexes);
  void tessellate(int subdivisions);

  bsp_vertex* m_vertexes;
  bsp_vertex m_control_vertexes[9];
  unsigned int* m_indexes;
  unsigned int* m_row_indexes[10];
  unsigned int m_tri_per_row[10];

  unsigned int m_vertex_offset;
  unsigned int m_index_offset;
};

#endif

