#include<stdio.h>
#include<GL/glew.h>
#define GLM_FORCE_RADIANS
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>

#define STEP 18
#define NumSpherePoints  STEP*(STEP-1)+2
#define NumSphereTriangles   STEP*(STEP-1)*2
#define NumSphereIndices     3*NumSphereTriangles

using namespace glm;

extern GLuint Sphere_vao;
extern GLuint Sphere_indices[];
extern vec4 Sphere_points[];
extern vec3 Sphere_normals[];


void createSphere();
void updateSphereVertexNormals();
void renderSphere();