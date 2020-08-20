#include<stdio.h>
#include<GL/glew.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <math.h>

#define NumConePoints  18
#define NumTriangles   18
#define NumIndices     3*NumTriangles

using namespace glm;

extern GLuint cone_vao;
extern GLuint cone_indices[];
extern vec4 cone_points[];
extern vec3 cone_normals[];


void createCone();
void updateVertexNormals();
void renderCone();