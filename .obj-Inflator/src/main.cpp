#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <math.h>
#include <vector>

#define GLM_FORCE_RADIANS 
#define TWOPI 2*3.141592653589793238

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include "objloader.h"
#include "shader.h"

using namespace glm;
using namespace std;

mat4 view;
mat4 model;
mat4 projection;
mat4 model_view;
GLuint program;
float aspect = 0.0;
GLfloat angle = 0.0;
GLuint vao;
unsigned int vbo[1];
GLuint ebo;

GLboolean showLines = false;

vector<vec3> vertices;
vector<vec3> normals; 
vector<int> indices;
int numverts;

vector<vec3> tempVertices;
vector<vec3> tempNormals;

static const double kPI = 3.1415926535897932384626433832795;
static const double amplitude = 0.06f;
static const double freq = 1.0f;
static const double initialAngle = 0.0f;
static const double D = 0.05f;


static int gViewportWidth, gViewportHeight;
vec4 light_position(10.0, 6.0, 8.0, 0.0);  // directional light source
GLfloat timeUpdate = 0.0;

void Initialize();
void Display(void);
void Reshape(int width, int height);
void updateTime();


void updateBunnyVertex(vector<vec3> &ptr, vector<vec3> verts, vector<vec3> &norms, int count);
void updateBunnyNormal(vector<vec3> &vNormals, vector<vec3> vertices, vector<int> indices);
/***************************************************************************/
void Initialize(void){

	// Create the program for rendering the model

	objloader loader;

	bool loadfile = loader.load("shrek.obj");
	vertices = loader.getVertices();
	normals = loader.getNormals();
	indices = loader.getVertexIndices();
	numverts = normals.size();
	tempVertices = vertices;
	tempNormals = normals;

	// Create and compile our GLSL program from the shaders
	program = LoadShaders("shader.vs", "shader.fs");

	glUseProgram(program);
	view = glm::lookAt(vec3(0.0f, 0.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = mat4(1.0f);

	// Initialize shader lighting parameters

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	GLuint offset = 0;
	glGenBuffers(1, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3) + normals.size() * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, offset, vertices.size()*sizeof(glm::vec3), &vertices[0]);
	offset += vertices.size()*sizeof(glm::vec3);
	glBufferSubData(GL_ARRAY_BUFFER, offset, normals.size() * sizeof(glm::vec3), &normals[0]);
	
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // Vertex position
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(vertices.size()*sizeof(glm::vec3)));
	glEnableVertexAttribArray(1);  // Vertex normal

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	vec4 lightintensity = vec4(0.9f, 0.9f, 0.9f, 1.0f);
	vec4 material_ambient(0.1, 0.55, 0.3, 1.0f);
	vec4 material_diffuse(0.1, 0.55, 0.3, 1.0f);
	vec4 material_specular(0.8, 0.8, 0.8, 1.0f);

	float material_shininess = 200.0;
	vec4 ambient_product = lightintensity*material_ambient;
	vec4 diffuse_product = lightintensity*material_diffuse;
	vec4 specular_product = lightintensity*material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, (GLfloat*)&ambient_product[0]);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, (GLfloat*)&diffuse_product[0]);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, (GLfloat*)&specular_product[0]);

	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, (GLfloat*)&light_position[0]);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	glClearColor(1.0, 1.0, 1.0, 1.0);
}

void Display(void)
{
	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

    glUseProgram(program);
	
	model = mat4(1.0f);
	
	model_view = view*model;
	mat3 normalmatrix;
	projection = glm::perspective(radians(60.0f), aspect, 0.3f, 100.0f); 
	mat4 mvp = projection*model_view;

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (GLfloat*)&model[0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, (GLfloat*)&view[0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, (GLfloat*)&projection[0]);
	
	updateTime();

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	//insert your code here
	updateBunnyVertex(tempVertices, vertices, normals, vertices.size());
	glBufferSubData(GL_ARRAY_BUFFER, 0, tempVertices.size() * sizeof(glm::vec3), &tempVertices[0]);
	//updateBunnyNormal(tempNormals, tempVertices, indices);
	//glBufferSubData(GL_ARRAY_BUFFER, tempVertices.size() * sizeof(glm::vec3), tempNormals.size() * sizeof(glm::vec3), &tempNormals[0]);

	if (showLines)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
	
	glBindVertexArray(0);

	glutSwapBuffers();
	//glutPostRedisplay();
}

void Reshape(int width, int height)
{
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);

	gViewportWidth = width;
	gViewportHeight = height;
	glViewport(0, 0, width, height);

	aspect = float(width) / float(height);
}

/****************************************************************************************/
void updateTime() {
	timeUpdate += 0.1f;
}
/**************************************************************************************/
void keyboard(unsigned char key, int x, int y){
	switch (key){
	case 'f':case 'F':
		// Toggles wireframe if presses 'f'
			showLines = !showLines;
		break;
	case 'P':case 'p':
		//toggles growth function
		printf("System paused...");
		system("Pause");
		break;
		
	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;
     }
	glutPostRedisplay();
}

void updater(int i) {
	glutPostRedisplay();
	glutTimerFunc(50, updater, 1);
}

/**********************************************************************************************************************************/
int main(int argc, char** argv){
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512, 512);
	
	glutCreateWindow(".obj Inflator     Author: Erik Almaguer");

	if (glewInit()){
		std::cerr << "Unable to initialize GLEW ... exiting" << std::endl;
	}
	
	Initialize();
	std::cout << glGetString(GL_VERSION) << std::endl;
	
	glutDisplayFunc(Display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(Reshape);
	glutTimerFunc(50,updater,1);
	glutMainLoop();
	return 0;
}

void updateBunnyNormal(vector<vec3> &vNormals, vector<vec3> vertices, vector<int> indices)
{
	// Complete this function
	vec3 P1, P2, P3;
	vec3 N;
	int i;// Calculate vertex normals
	for (i = 0; i < indices.size(); i += 3) {
		P1 = vertices[indices[i + 0]];
		P2 = vertices[indices[i + 1]];
		P3 = vertices[indices[i + 2]];
		N = normalize(cross(vec3(P2 - P1), (vec3(P3 - P1))));
		vNormals[indices[i + 0]] += N;
		vNormals[indices[i + 1]] += N;
		vNormals[indices[i + 2]] += N;
	}
	//normalize normals
	for (i = 0; i < vertices.size(); i++)vNormals[i] = normalize(vec3(vNormals[i]));
}

void updateBunnyVertex(vector<vec3> &ptr, vector<vec3> verts, vector<vec3> &norms, int count){
	
	// Complete this function
	float F = amplitude * sin((2 * kPI * freq * timeUpdate) + initialAngle) + D;
	
	for (int i = 0; i < verts.size(); i++) {
		ptr[i] = verts[i] + F * norms[i];
	}

}


