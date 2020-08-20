#include<stdio.h>
#include<GL/glew.h>
#include <glm/mat4x4.hpp> // glm::mat4
#include <math.h>

#define NumHexagonPoints  6
#define NumHexagonTriangles   6
#define NumHexagonIndices     3*NumHexagonTriangles

using namespace glm;

extern GLuint Hexagon_vao;
extern GLuint Hexagon_indices[];
extern vec4 Hexagon_points[];
extern vec3 Hexagon_normals[];


void createHexagon();
void updateHexagonVertexNormals();
void renderHexagon();