#include<stdio.h>
#include<GL/glew.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <math.h>

#define NumConePoints  18
#define NumConeTriangles   18
#define NumConeIndices     3*NumConeTriangles

using namespace glm;

extern GLuint Cone_vao;
extern GLuint Cone_indices[];
extern vec4 Cone_points[];
extern vec3 Cone_normals[];


void createCone();
void updateConeVertexNormals();
void renderCone();