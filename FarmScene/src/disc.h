#include<stdio.h>
#include<GL/glew.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <math.h>

#define NumDiscPoints  18
#define NumDiscTriangles   18
#define NumDiscIndices     3*NumDiscTriangles

using namespace glm;

extern GLuint disc_vao;
extern GLuint disc_indices[];
extern vec4 disc_points[];
extern vec3 disc_normals[];


void createDisc();
void updateDiscVertexNormals();
void renderDisc();