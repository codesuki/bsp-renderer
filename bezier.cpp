#include "bezier.h"

bezier::bezier(void)
{
	m_vertexes = NULL;
	m_indexes = NULL;
}

bezier::~bezier(void)
{
	SAFE_DELETE_ARRAY(m_vertexes)
	SAFE_DELETE_ARRAY(m_indexes)
}


// TODO: for adding color interpolation maybe check for t < 0.5 and use 1+2 or 2+3 vertex accordingly
bsp_vertex bezier::calculate_quadratic_bezier(float t, const bsp_vertex* control_vertexes)
{
	// http://de.wikipedia.org/wiki/B%C3%A9zierkurve#Kubische_B.C3.A9zierkurven_.28n.3D3.29
	// quadratic bezier curve (n=2)

	//bsp_vertex a = control_vertexes[0];
	//a.color[0] = 255; 
	//a.color[1] = 0;
	//a.color[2] = 0;
	//a.color[3] = 255;
	//bsp_vertex b = control_vertexes[1];
	//b.color[0] = 0; 
	//b.color[1] = 255;
	//b.color[2] = 0;
	//b.color[3] = 255;
	//bsp_vertex c = control_vertexes[2];
	//c.color[0] = 0; 
	//c.color[1] = 0;
	//c.color[2] = 255;
	//c.color[3] = 255;
	//return (a)*(1-t)*(1-t)+(b)*2*t*(1-t)+(c)*t*t;
	//bsp_vertex vert = (control_vertexes[0])*(1-t)*(1-t)+(control_vertexes[1])*2*t*(1-t)+(control_vertexes[2])*t*t;
	//if (t < 0.5f) {
	//	vert.color[0] = (1-t)*(control_vertexes[0].color[0])+t*(control_vertexes[1].color[0]);
	//	vert.color[1] = (1-t)*(control_vertexes[0].color[1])+t*(control_vertexes[1].color[1]);
	//	vert.color[2] = (1-t)*(control_vertexes[0].color[2])+t*(control_vertexes[1].color[2]);
	//	vert.color[3] = (1-t)*(control_vertexes[0].color[3])+t*(control_vertexes[1].color[3]);
	//} else {
	//	vert.color[0] = (1-t)*(control_vertexes[1].color[0])+t*(control_vertexes[2].color[0]);
	//	vert.color[1] = (1-t)*(control_vertexes[1].color[1])+t*(control_vertexes[2].color[1]);
	//	vert.color[2] = (1-t)*(control_vertexes[1].color[2])+t*(control_vertexes[2].color[2]);
	//	vert.color[3] = (1-t)*(control_vertexes[1].color[3])+t*(control_vertexes[2].color[3]);
	//}

	return (control_vertexes[0])*(1-t)*(1-t)+(control_vertexes[1])*2*t*(1-t)+(control_vertexes[2])*t*t;
}

void bezier::tessellate(int subdivisions)
{
  bsp_vertex temp[3];
  int subdivisions1 = subdivisions+1;

  if (m_vertexes != NULL) delete [] m_vertexes;
  m_vertexes = new bsp_vertex[subdivisions1*subdivisions1];

  int difference = subdivisions1 / 3;

  int row = 0;

  // this loop saves the 1. loop for getting the 3 temp control vertexes
  // TODO: maybe remove or add more enhancements :p 
  //for (int i = 0; i <= subdivisions; ++i) {
  //	float a = (double)i / subdivisions;
  //	float b = 1 - a;

  //	m_vertexes[i] =
  //		m_control_vertexes[0] * (b * b) +
  //		m_control_vertexes[3] * (2 * b * a) +
  //		m_control_vertexes[6] * (a * a);

  //	//m_vertexes[i].color[0] = 0xaa;
  //	//m_vertexes[i].color[1] = 0xaa;
  //	//m_vertexes[i].color[2] = 0xaa;
  //	//m_vertexes[i].color[3] = 0xaa;
  //}

  for (int i = 0; i <= subdivisions; ++i) 
  {
    float l = (float)i/subdivisions;

    for (int j = 0; j < 3; ++j) 
    {
      int k = 3 * j;
      temp[j] = calculate_quadratic_bezier(l, &(m_control_vertexes[k]));
    }

    int col = 0;
    for(int j = 0; j <= subdivisions; ++j)
    {
      float a = (float)j / subdivisions;

      m_vertexes[i * subdivisions1 + j] = calculate_quadratic_bezier(a, temp);
      m_vertexes[i * subdivisions1 + j].color[0] = 0xff;
      m_vertexes[i * subdivisions1 + j].color[1] = 0xff;
      m_vertexes[i * subdivisions1 + j].color[2] = 0xff;
      m_vertexes[i * subdivisions1 + j].color[3] = 0xff;
    }
  }

  if (m_indexes != NULL) delete [] m_indexes;
  m_indexes = new unsigned int[subdivisions * subdivisions1 * 2];

  // maybe use degenerated triangle strips to merge
  for (row = 0; row < subdivisions; ++row)
  {
    for(int col = 0; col <= subdivisions; ++col)	{
      int g = (row * (subdivisions1) + col) * 2 + 1;
      int h = (row * (subdivisions1) + col) * 2;
      m_indexes[g] = row       * subdivisions1 + col;
      m_indexes[h] = (row + 1) * subdivisions1 + col;
    }
  }

  for (row = 0; row < subdivisions; ++row) 
  {
    m_tri_per_row[row] = 2 * subdivisions1;
    m_row_indexes[row] = &(m_indexes[row * 2 * subdivisions1]);
  }
}

