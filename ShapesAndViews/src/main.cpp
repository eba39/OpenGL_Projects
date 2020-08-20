#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Triangle.h"
#include "pyramid.h"
#include "disc.h"
#include "hexagon.h"
#include "cone.h"
#include "sphere.h"
#include "cube.h"

#define GLM_FORCE_RADIANS
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>

using namespace glm;

GLuint v, f;
GLboolean show_line = false;
GLboolean rotate_op = true;
GLboolean orthographic = true;
float aspect = 0.0;
GLfloat angle = 0.0f;
GLfloat current_angle = angle;
GLfloat angle_add = 1.0f;
GLuint model_matrix_loc;
GLuint view_matrix_loc;
GLuint projection_matrix_loc;
mat4 model_matrix;
mat4 view_matrix;
mat4 projection_matrix;

GLuint program;
GLuint smooth_prog;

//lighting stuff
float material_shininess = 5.0; 
vec4 light_position(10.0, 15.0, 9.8, 1.0); 
vec4 light_ambient(0.3, 0.3, 0.3, 1.0);
vec4 light_diffuse(1.0, 1.0, 1.0, 1.0); 
vec4 light_specular(1.0, 1.0, 1.0, 1.0); 
vec4 material_ambient(0.0, 0.5, 1.0, 1.0); 
vec4 material_diffuse(0.0, 0.5, 1.0, 1.0);
vec4 material_specular(0.0, 0.5, 1.0, 1.0);
mat3 normal;
mat4 view;
mat4 model;

// position of the vertices in the hexagon

/*--------------------------------------------------------------------------------------*/

const GLchar* ReadShader(const char* filename);
GLuint LoadShaders(char* v_shader, char* f_shader);
void init(void);
void display(void);
void keyboard(unsigned char key, int x, int y);
//----------------------------------------------------------------------------

const GLchar* ReadShader(const char* filename) {
#ifdef WIN32
	FILE* infile;
	fopen_s(&infile, filename, "rb");

#else
	FILE* infile = fopen(filename, "rb");
#endif // WIN32

	if (!infile) {
#ifdef _DEBUG
		printf("Unable to open file %s", filename);
#endif /* DEBUG */
		return NULL;
	}

	fseek(infile, 0, SEEK_END);
	int len = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	GLchar* source = (GLchar*)malloc(sizeof(GLchar)*(len + 1));

	fread(source, 1, len, infile);
	fclose(infile);

	source[len] = 0;

	return ((GLchar*)source);
}

//----------------------------------------------------------------------------

GLuint LoadShaders(char* v_shader, char* f_shader) {

	GLuint p = glCreateProgram();
	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	const char * vs = ReadShader(v_shader);
	const char * fs = ReadShader(f_shader);

	glShaderSource(v, 1, &vs, NULL);
	glShaderSource(f, 1, &fs, NULL);
	free((char*)vs);
	free((char*)fs);

	glCompileShader(v);
	GLint compiled;
	glGetShaderiv(v, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLsizei len;
		glGetShaderiv(v, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = (GLchar*)malloc((sizeof(GLchar))*(len + 1));
		glGetShaderInfoLog(v, len, &len, log);
		printf("Vertex Shader compilation failed: %s\n", log);
		free((GLchar*)log);
	}

	glCompileShader(f);

	glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLsizei len;
		glGetShaderiv(f, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = (GLchar*)malloc((sizeof(GLchar))*(len + 1));
		glGetShaderInfoLog(f, len, &len, log);
		printf("Fragment Shader compilation failed: %s\n", log);
		free((GLchar*)log);
	}

	glAttachShader(p, v);
	glAttachShader(p, f);
	glBindAttribLocation(p, 0, "vPosition");


	glLinkProgram(p);

	GLint linked;
	glGetProgramiv(p, GL_LINK_STATUS, &linked);
	if (!linked) {
		GLsizei len;
		glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = (GLchar*)malloc(sizeof(GLchar)*(len + 1));
		glGetProgramInfoLog(p, len, &len, log);
		printf("Shader linking failed: %s\n", log);
		free((GLchar*)log);
	}

	return p;

}
////////////////////////////////////////////////////////////////////////////////////////
/********************************************************************************/

void init(void) {
	//creating shapes for rendering
	createDisc();
	createTriangle();
	createPyramid();
	createHexagon();
	createCone();
	createSphere();
	createCube();

	program = LoadShaders((char*)"shader.vert", (char*)"shader.frag");
	/*glUseProgram(program);

	model_matrix_loc = glGetUniformLocation(program, "model_matrix");
	projection_matrix_loc = glGetUniformLocation(program, "projection_matrix");
	view_matrix_loc = glGetUniformLocation(program, "view_matrix");

	projection_matrix = ortho(-3.0f, 3.0f, -3.0f, 3.0f, -3.0f, 3.0f);
	//projection_matrix = perspective(radians(70.0f), aspect, 0.3f, 100.0f);
	//projection_matrix = frustum(-4.0f, 4.0f, -4.0f, 4.0f, 4.3f, 100.0f);
	view_matrix = lookAt(vec3(0.0f, 0.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, (GLfloat*)&projection_matrix);
	glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, (GLfloat*)&view_matrix);
	
	  // positional light source
	vec4 lightpos = view_matrix * light_position;
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, (GLfloat*)&lightpos);*/

	smooth_prog = LoadShaders((char*)"smoothshader.vert", (char*)"smoothshader.frag");
	glUseProgram(smooth_prog);
	vec4 lightpos = view_matrix * light_position;
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, (GLfloat*)&lightpos);
	view = glm::lookAt(vec3(0.0f, 0.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.75, 0.75, 0.75, 1.0);

}

void setMatrices() {
	glUseProgram(smooth_prog);

	model = model_matrix;
	vec4 ambient_product = light_ambient * material_ambient;
	vec4 diffuse_product = light_diffuse * material_diffuse;
	vec4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(smooth_prog, "AmbientProduct"), 1, (GLfloat*)&ambient_product[0]);
	glUniform4fv(glGetUniformLocation(smooth_prog, "DiffuseProduct"), 1, (GLfloat*)&diffuse_product[0]);
	glUniform4fv(glGetUniformLocation(smooth_prog, "SpecularProduct"), 1, (GLfloat*)&specular_product[0]);
	glUniform1f(glGetUniformLocation(smooth_prog, "Shininess"), material_shininess);

	/*LIGHTING*/
	/*LIGHTING*/

	/*VIEW*/
	if (orthographic)
		projection_matrix = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 100.0f);
	else
		projection_matrix = glm::perspective(radians(70.0f), aspect, 0.1f, 100.0f);
	/*VIEW*/

	mat4 mvp = projection_matrix * view*model;
	mat4 model_view = view * model;

	mat3 normal = mat3(view*model);
	glUniformMatrix4fv(glGetUniformLocation(smooth_prog, "MVP"), 1, GL_FALSE, (GLfloat*)&mvp[0]);
	glUniformMatrix4fv(glGetUniformLocation(smooth_prog, "ProjectionMatrix"), 1, GL_FALSE, (GLfloat*)&projection_matrix[0]);
	glUniformMatrix3fv(glGetUniformLocation(smooth_prog, "NormalMatrix"), 1, GL_FALSE, (GLfloat*)&normal[0]);
	glUniformMatrix4fv(glGetUniformLocation(smooth_prog, "ModelViewMatrix"), 1, GL_FALSE, (GLfloat*)&model_view[0]);
}

/****************************************************************************************/
void display(void) {
	/*LIGHTING*/
	/*LIGHTING*/
	
	//view_matrix = lookAt(vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (show_line)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	

	if (rotate_op)
		current_angle = angle;

	model_matrix = translate(mat4(1.0f), glm::vec3(-1.85f, 1.85f, 0.0f));
	model_matrix *= rotate(mat4(1.0f), radians(current_angle), vec3(1.0f, 0.5f, 0.0f));
	material_ambient = vec4(0.0, 0.5, 1.0, 1.0);
	material_diffuse = vec4(0.0, 0.5, 1.0, 1.0);
	material_specular = vec4(0.0, 0.5, 1.0, 1.0);
	setMatrices();
	renderSphere();

	model_matrix = mat4();
	model_matrix = translate(mat4(1.0f), glm::vec3(1.85f, 1.85f, 0.0f));
	model_matrix *= rotate(mat4(1.0f), radians(current_angle), vec3(0.5f, 1.0f, 0.0f));
	model_matrix *= scale(glm::mat4(1.0f), glm::vec3(0.75f, 0.75f, 0.75f));
	material_ambient = vec4(1.0, 0.5, 0.0, 1.0);
	material_diffuse = vec4(1.0, 0.5, 0.0, 1.0);
	material_specular = vec4(1.0, 0.5, 0.0, 1.0);
	setMatrices();
	renderPyramid();

	model_matrix = mat4();
	model_matrix = translate(mat4(1.0f), glm::vec3(1.85f, -1.85f, 0.0f));
	model_matrix *= rotate(mat4(1.0f), radians(180.0f), vec3(1.0f, 0.0f, 0.0f));
	model_matrix *= rotate(mat4(1.0f), radians(current_angle), vec3(0.0f, 0.0f, 1.0f));
	model_matrix *= scale(glm::mat4(1.0f), glm::vec3(0.75f, 0.75f, 0.75f));
	material_ambient = vec4(0.0, 1.0, 0.0, 1.0);
	material_diffuse = vec4(0.0, 1.0, 0.0, 1.0);
	material_specular = vec4(0.0, 1.0, 0.0, 1.0);
	setMatrices();
	renderDisc();

	model_matrix = mat4();
	model_matrix = translate(mat4(1.0f), glm::vec3(0.0f, -1.85f, 0.0f));
	model_matrix *= rotate(mat4(1.0f), radians(180.0f), vec3(1.0f, 0.0f, 0.0f));
	model_matrix *= rotate(mat4(1.0f), radians(current_angle), vec3(0.0f, 0.0f, 1.0f));
	model_matrix *= scale(glm::mat4(1.0f), glm::vec3(0.75f, 0.75f, 0.75f));
	material_ambient = vec4(0.7, 0.0, 0.3, 1.0);
	material_diffuse = vec4(0.7, 0.0, 0.3, 1.0);
	material_specular = vec4(0.7, 0.0, 0.3, 1.0);
	setMatrices();
	renderHexagon();

	model_matrix = mat4();
	model_matrix = translate(mat4(1.0f), glm::vec3(-1.75f, -1.5f, 0.0f));
	model_matrix *= rotate(mat4(1.0f), radians(-current_angle), vec3(1.0f, 0.5f, 0.0f));
	model_matrix *= scale(glm::mat4(1.0f), glm::vec3(0.75f, 0.75f, 0.75f));
	material_ambient = vec4(0.3, 0.0, 1.0, 1.0);
	material_diffuse = vec4(0.3, 0.0, 1.0, 1.0);
	material_specular = vec4(0.3, 0.0, 1.0, 1.0);
	setMatrices();
	renderCone();

	model_matrix = mat4();
	model_matrix *= rotate(mat4(1.0f), radians(-current_angle), vec3(-1.0f, 0.5f, 0.0f));
	material_ambient = vec4(1.0, 0.0, 0.0, 1.0);
	material_diffuse = vec4(1.0, 0.0, 0.0, 1.0);
	material_specular = vec4(1.0, 0.0, 0.0, 1.0);
	setMatrices();
	renderCube();

	glFlush();
}

/*******************************************************************************************************************/

void rotate_func(int i) {

	angle += angle_add;
	glutPostRedisplay();
	glutTimerFunc(20, rotate_func, 1);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'l':
	case 'L':
		show_line = !show_line;
		break;
	case 'r':
		rotate_op = !rotate_op;
		printf("\nrotate_op = %d", rotate_op);
		break;
	case 'o':
	case 'O':
		orthographic = !orthographic;

	}
	glutPostRedisplay();
}

/*********************************************************************************************/

void Reshape(int width, int height) {

	glViewport(0, 0, width, height);

	aspect = float(width) / float(height);
}

/*******************************************************************************************************************/
int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("Drawing objects     Author: Erik Almaguer");

	if (glewInit()) {
		printf("Unable to initialize GLEW ... exiting\n");
	}

	init();
	printf("%s", glGetString(GL_VERSION));
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(Reshape);
	glutTimerFunc(20, rotate_func, 1);
	glutMainLoop();
}

/*****/
