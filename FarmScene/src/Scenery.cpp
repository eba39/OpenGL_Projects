// AUTHOR: ERIK ALMAGUER
// DATE: MARCH 2018
// COURSE: CSCD 396
// FINAL-PROJECT

#include <GL/glew.h>
#include <GL/freeglut.h>
#include<string>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include <stdlib.h>
#include <iostream>
#include <math.h>


//shape headers
#include "grass.h"
#include "ground.h"
#include "box.h"
#include "pyramid.h"
#include "disc.h"
#include "Plane.h"
#include "cone.h"



#define GLM_FORCE_RADIANS 

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 

using namespace glm;

void Initialize(void);
void Display(void);

//keyboard variables
//GLfloat box_pos[] = { -25.0f, 50.0f, 100.0f };
GLfloat box_pos[] = { 2.0f, 45.5f, 182.25f };
GLfloat box_scale[] = { 150.0f, 150.0f, 150.0f };
GLfloat box_rot[] = { 0.0f, 0.0f, 0.0f };

//cow variables
GLfloat cowCoordinates[4][3] = { {-14.0f, 36.0f, 160.0f},
								 {14.0f, 163.0f, 37.0f },
								 {-14.0f, 163.0f, 36.0f },
								 {14.0f, 163.0f, 37.0f }
};

GLfloat cowScale[4][3] = { { 10.0f, 7.0f, 7.0f },
						   { 10.0f, 7.0f, 7.0f }, 
						   { 10.0f, 7.0f, 7.0f }, 
						   { 10.0f, 7.0f, 7.0f }, };

GLboolean cowLife[4] = { true, true, true, true };

//sky rotation
GLfloat skyAngle = 0.0f;
GLfloat skyAngle_add = 0.25f;
GLfloat threeSixty = 360.0f;
GLfloat skyRGB[] = { 0.5f, 0.75f, 0.85f, 1.0f };

//scale and position values for spaceship
GLfloat tractor_scale[] = { 0.3f, 0.0f, 0.3 };
GLfloat ufo_rotation = 0.0f;
GLfloat ufo_pos[3] = { 2.0f, 45.5f, 182.25f };
GLboolean tractor_mode = false;
GLboolean trac_col_run = false;
int lastCow = -1;
GLboolean cowInHold = false;
GLfloat p_pos[] = { 0.0f, 0.0f };
GLfloat mover_value = 0.025f;
GLfloat p_pos_inc[] = { -mover_value , -mover_value };

//windmill vars
GLfloat windmillPos[] = {-23.5f, 44.0f, 143.0f};
GLfloat windmillAngle = 0.0f;
GLfloat windmillSpeed = 1.0f;

//tree vars
GLfloat treesPos[7][3] = { {-35.5f, 40.25f, 148.5f} };
GLfloat waves[7];
GLboolean treeWaves[7] = {true};

//texture ID's
GLuint planeTexID;
GLuint boxTexID;
GLuint skyboxTexID;
GLuint discTexID;
GLuint tractorbeamTexID;
GLuint earthTexID;
GLuint dayAndNightID;
GLuint sunTexID;
GLuint moonTexID;
GLuint cowTexID[4];
GLuint ufoTexID[4];
GLuint windmillTexID[2];
GLuint treeTexID[2];
GLuint houseTexID[2];

//shader program id's
GLuint prog;
GLuint render_prog;
GLuint texture_prog;

typedef struct {
	GLenum       type;
	const char*  filename;
	GLuint       shader;
} ShaderInfo;


float angle = 0.0;

vec4 light_position(20.0, 40.0, 60.0, 0.0);  // directional light source

float material_shininess = 50.0;

vec4 ambient_product;
vec4 diffuse_product;
vec4 specular_product;

mat4 view_matrix;
mat4 projection_matrix;
mat4 model_matrix;
GLuint model_matrix_loc;
GLuint view_matrix_loc;
GLuint projection_matrix_loc;
float aspect;

unsigned int loadTexture(char* filename);
void setTextureMatrix();
void cowSetter();
void treeSetter();

//----------------------------------------------------------------------------//
float RandomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}
//----------------------------------------------------------------------------//

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

//----------------------------------------------------------------------------//

GLuint LoadShaders(ShaderInfo* shaders){
	if (shaders == NULL) { return 0; }

	GLuint program = glCreateProgram();

	ShaderInfo* entry = shaders;
	while (entry->type != GL_NONE) {
		GLuint shader = glCreateShader(entry->type);

		entry->shader = shader;

		const GLchar* source = ReadShader(entry->filename);

		if (source == NULL) {


			for (entry = shaders; entry->type != GL_NONE; ++entry) {
				glDeleteShader(entry->shader);
				entry->shader = 0;
			}

			return 0;
		}

		glShaderSource(shader, 1, &source, NULL);
		free((GLchar*)source);

		glCompileShader(shader);

		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
#ifdef _DEBUG
			GLsizei len;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

			GLchar* log = (GLchar*)malloc(sizeof(GLchar)*(len + 1));
			glGetShaderInfoLog(shader, len, &len, log);
			printf("Shader compilation failed.\n");
			free(log);
#endif /* DEBUG */

			return 0;
		}

		glAttachShader(program, shader);

		++entry;
	}

	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
#ifdef _DEBUG
		GLsizei len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = (GLchar*)malloc(sizeof(GLchar)*(len + 1));
		glGetProgramInfoLog(program, len, &len, log);
		printf("Shader linking failed: %s\n", log);
		free(log);
#endif /* DEBUG */

		for (entry = shaders; entry->type != GL_NONE; ++entry) {
			glDeleteShader(entry->shader);
			entry->shader = 0;
		}

		return 0;
	}

	return program;
}
////////////////////////////////////////////////////////////////////////////////////////
/********************************************************************************/

unsigned int loadTexture(char* filename) {

	ILboolean success;
	unsigned int imageID;


	// Load Texture Map
	ilGenImages(1, &imageID);

	ilBindImage(imageID); /* Binding of DevIL image name */
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	success = ilLoadImage((ILstring)filename);

	if (!success) {
		printf("Couldn't load the following texture file: %s", filename);
		// The operation was not sucessfull hence free image and texture 
		ilDeleteImages(1, &imageID);
		return 0;
	}

	// add information to the log
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	GLuint tid;
	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);


	glBindTexture(GL_TEXTURE_2D, 0);

	/* Because we have already copied image data into texture data
	we can release memory used by image. */
	ilDeleteImages(1, &imageID);
	return tid;
}


void Reshape(int width, int height) {

	glViewport(0, 0, width, height);
	aspect = float(width) / float(height);
}
/*****************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////

/*********************************************************/
/*                  INITIALIZE SHADERS                   */
/*               INITIALIZE TEXTURE ID's                 */
/*                  INITIALIZE SHADERS                   */
/*                   CREATE 3D SHAPES                    */
/*********************************************************/
void Initialize(void)
{
	// Create the program for rendering the model
	
	// Initialize shader lighting parameters

	//grass shader
	ShaderInfo shader[] = { { GL_VERTEX_SHADER, "grassOne.VERT" },
	{ GL_FRAGMENT_SHADER, "grassOne.FRAG" },
	{ GL_NONE, NULL }
	};

	
	render_prog = LoadShaders(shader);
	glUseProgram(render_prog);
	

	//texture shader
	ShaderInfo texture_shader[] = { { GL_VERTEX_SHADER, "texture.vs" },
	{ GL_FRAGMENT_SHADER, "texture.fs" },
	{ GL_NONE, NULL }
	};

	//initializing texture shader
	texture_prog = LoadShaders(texture_shader);
	glUseProgram(texture_prog);

	vec3 light_intensity(1.0f, 1.0f, 1.0f);
	vec4 light_position(10.0f, 10.0f, 10.0f, 1.0f);
	vec3 material_ambient(0.5, 0.5, 0.5);
	vec3 material_diffuse(0.9, 0.9, 0.9);
	vec3 material_specular(0.9, 0.9, 0.9);

	GLfloat shininess = 100.0f;

	glUniform3fv(glGetUniformLocation(texture_prog, "Light.Intensity"), 1, (GLfloat*)&light_intensity);
	glUniform4fv(glGetUniformLocation(texture_prog, "Light.Position"), 1, (GLfloat*)&light_position);
	glUniform3fv(glGetUniformLocation(texture_prog, "Material.Ka"), 1, (GLfloat*)&material_ambient);
	glUniform3fv(glGetUniformLocation(texture_prog, "Material.Kd"), 1, (GLfloat*)&material_diffuse[0]);
	glUniform3fv(glGetUniformLocation(texture_prog, "Material.Ks"), 1, (GLfloat*)&material_specular[0]);
	glUniform1f(glGetUniformLocation(texture_prog, "Material.Shininess"), shininess);
	glUniform1i(glGetUniformLocation(texture_prog, "Tex1"), 0);


	//common shader
	ShaderInfo common_shaders[] = { { GL_VERTEX_SHADER, "common.vs" },
	                                { GL_FRAGMENT_SHADER, "common.fs" },
	                                { GL_NONE, NULL } };

	prog = LoadShaders(common_shaders);

	glUseProgram(prog);

	
	
	//loadiing textures
	planeTexID = loadTexture((char*)"brick.jpg");
	boxTexID = loadTexture((char*)"alien.jpg");
	skyboxTexID = loadTexture((char*)"deepfield.jpg");
	discTexID = loadTexture((char*)"win-xp.jpg");
	tractorbeamTexID = loadTexture((char*)"tractorbeam.jpg");
	earthTexID = loadTexture((char*)"earth.jpg");
	dayAndNightID = loadTexture((char*)"sky.jpg");
	moonTexID = loadTexture((char*)"moon.jpg");
	sunTexID = loadTexture((char*)"sun.jpg");
	cowTexID[0] = loadTexture((char*)"cow1.jpg");
	cowTexID[1] = loadTexture((char*)"cow2.jpg");
	cowTexID[2] = loadTexture((char*)"cow3.jpg");
	cowTexID[3] = loadTexture((char*)"cow4.jpg");
	ufoTexID[0] = loadTexture((char*)"alienreddot.jpg");
	ufoTexID[1] = loadTexture((char*)"alienbluedot.jpg");
	ufoTexID[2] = loadTexture((char*)"shiptex.jpg");
	windmillTexID[0] = loadTexture((char*)"windmillBody.jpg");
	windmillTexID[1] = loadTexture((char*)"windmillBase.jpg");
	treeTexID[0] = loadTexture((char*)"treestump.jpg");
	treeTexID[1] = loadTexture((char*)"treetop.jpg");
	houseTexID[0] = loadTexture((char*)"houseTex.jpg");
	houseTexID[1] = loadTexture((char*)"houseRoof.jpg");
	
	
	//creating 3d shapes
	createGrass();
	createBox();
	createGround();
	createPyramid();
	createDisc();
	createPlane();
	createCone();
	cowSetter();
	treeSetter();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, planeTexID);

	glClearColor(skyRGB[0], skyRGB[1], skyRGB[2], skyRGB[3]);  // sky color
}

void setTextureMatrix() {

	mat4 model_view;
	mat3 normalmatrix = mat3(1.0f);
	mat4 mvp = mat4(1.0f);
	mat4 projection = glm::perspective(radians(70.0f), aspect, 0.3f, 100.0f);

	view_matrix = glm::lookAt(glm::vec3(0.0f, 40.0f, 200.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model_view = view_matrix * model_matrix;
	normalmatrix = mat3(vec3(model_view[0]), vec3(model_view[1]), vec3(model_view[2]));
	mvp = projection * model_view;

	glUniformMatrix4fv(glGetUniformLocation(texture_prog, "ModelViewMatrix"), 1, GL_FALSE, (GLfloat*)&model_view[0]);
	glUniformMatrix3fv(glGetUniformLocation(texture_prog, "NormalMatrix"), 1, GL_FALSE, (GLfloat*)&normalmatrix[0]);
	glUniformMatrix4fv(glGetUniformLocation(texture_prog, "MVP"), 1, GL_FALSE, (GLfloat*)&mvp[0]);
	glUniformMatrix4fv(glGetUniformLocation(texture_prog, "ProjectionMatrix"), 1, GL_FALSE, (GLfloat*)&projection[0]);


}

void cowSetter() {
	//cow_selector = -1;

	for (int i = 0; i < (sizeof(cowCoordinates) / sizeof(cowCoordinates[0])); i++) {
		cowCoordinates[i][0] = RandomFloat(-20.5f, 20.5f);
		cowCoordinates[i][1] = 36.0f;
		cowCoordinates[i][2] = RandomFloat(147.5f, 183.75f);
		cowScale[i][0] = 10.0f;
		cowScale[i][1] = 7.0f;
		cowScale[i][2] = 7.0f;
		cowLife[i] = true;
	}
	
}

void treeSetter() {
	GLfloat tree_mover = 10.0f;
	GLboolean zigzag = false;

	for (int i = 1; i < (sizeof(treesPos) / sizeof(treesPos[0])); i++) {
		treesPos[i][0] = treesPos[0][0] + (i*tree_mover);
		treesPos[i][1] = treesPos[0][1] + (zigzag ? -4.0f : 0.0f);
		treesPos[i][2] = treesPos[0][2] + (zigzag ? 15.0f : 0.0f);
		treeWaves[i] = (zigzag ? true : false);
		waves[i] = RandomFloat(3.0f, -3.0f);
		zigzag = !zigzag;
	}
}

/*********************************************************/
/*             GRASS AND GROUND CONTROLLER               */
/*********************************************************/
void grassGroundController() {
	glUseProgram(render_prog);

	//initializing grass shader
	vec4 light_ambient(0.5, 0.5, 1.0, 1.0);
	vec4 light_diffuse(0.5, 0.5, 1.0, 1.0);
	vec4 light_specular(1.0, 1.0, 1.0, 1.0);

	vec4 material_ambient(0.1, 1.0, 0.1, 1.0);
	vec4 material_diffuse(0.1, 1.0, 0.1, 1.0);
	vec4 material_specular(1.0, 1.0, 1.0, 1.0);

	ambient_product = light_ambient * material_ambient;
	diffuse_product = light_diffuse * material_diffuse;
	specular_product = light_specular * material_specular;

	//vec3 light_intensity(1.0f, 1.0f, 1.0f);
	//if (skyAngle < 180)
		//light_intensity = vec3(0.7f, 0.7f, 1.0f);

	//vec4 light_position(70.0f*cos(radians(-skyAngle)), 40.0f*sin(radians(-skyAngle)), 1.0f, 100.0f);

	glUniform4fv(glGetUniformLocation(render_prog, "AmbientProduct"), 1, (GLfloat*)&ambient_product[0]);
	glUniform4fv(glGetUniformLocation(render_prog, "DiffuseProduct"), 1, (GLfloat*)&diffuse_product[0]);
	glUniform4fv(glGetUniformLocation(render_prog, "SpecularProduct"), 1, (GLfloat*)&specular_product[0]);

	glUniform4fv(glGetUniformLocation(render_prog, "LightPosition"), 1, (GLfloat*)&light_position[0]);
	glUniform1f(glGetUniformLocation(render_prog, "Shininess"), material_shininess);

	view_matrix_loc = glGetUniformLocation(render_prog, "view_matrix");
	projection_matrix_loc = glGetUniformLocation(render_prog, "projection_matrix");
	model_matrix_loc = glGetUniformLocation(render_prog, "model_matrix");

	view_matrix = glm::lookAt(glm::vec3(0.0f, 40.0f, 200.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, (GLfloat*)&view_matrix[0]);
	projection_matrix = perspective(radians(60.0f), aspect, 1.0f, 3000.0f);
	model_matrix = rotate(mat4(1.0f), radians(0.0f), vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, (GLfloat*)&projection_matrix[0]);

	renderGrass();

	glUseProgram(prog);

	material_ambient = vec4(0.25, 0.25, 0.25, 1.0);
	material_diffuse = vec4(0.25, 0.25, 0.25, 1.0);
	material_specular = vec4(0.25, 0.25, 0.25, 1.0);

	ambient_product = light_ambient * material_ambient;
	diffuse_product = light_diffuse * material_diffuse;
	specular_product = light_specular * material_specular;
	glUniform4fv(glGetUniformLocation(prog, "AmbientProduct"), 1, (GLfloat*)&ambient_product[0]);
	glUniform4fv(glGetUniformLocation(prog, "DiffuseProduct"), 1, (GLfloat*)&diffuse_product[0]);
	glUniform4fv(glGetUniformLocation(prog, "SpecularProduct"), 1, (GLfloat*)&specular_product[0]);

	glUniform4fv(glGetUniformLocation(prog, "LightPosition"), 1, (GLfloat*)&light_position[0]);
	glUniform1f(glGetUniformLocation(prog, "Shininess"), material_shininess);

	view_matrix_loc = glGetUniformLocation(prog, "view_matrix");
	projection_matrix_loc = glGetUniformLocation(prog, "projection_matrix");
	model_matrix_loc = glGetUniformLocation(prog, "model_matrix");
	view_matrix = glm::lookAt(glm::vec3(0.0f, 40.0f, 200.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	projection_matrix = perspective(radians(60.0f), aspect, 1.0f, 3000.0f);

	glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, (GLfloat*)&view_matrix[0]);
	glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, (GLfloat*)&projection_matrix[0]);


	renderGround();
}

/*********************************************************/
/*                   HOUSE CONTROLLER                    */
/*********************************************************/
void houseController() {


	//mat4 housePos = glm::translate(glm::mat4(1.0f), glm::vec3(box_pos[0], box_pos[1], box_pos[2]));
	mat4 housePos = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	//house matrices
	glUseProgram(texture_prog);
	model_matrix = mat4(1.0f);
	mat4 mat2_scaled, mat2_translated, saucer_pos, initial_rotation;
	initial_rotation = glm::rotate(mat4(1.0f), radians(9.0f), vec3(1.0f, 0.0f, 0.0f));


	//cube transformations
	mat2_scaled = glm::scale(glm::mat4(1.0f), glm::vec3(25.0f, 25.0f, 25.0f));
	mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 36.0f, 160.0f));

	model_matrix = housePos * mat2_translated * initial_rotation * mat2_scaled;

	setTextureMatrix();
	glBindTexture(GL_TEXTURE_2D, houseTexID[0]);
	renderBox();
	

	//pyramid transformations
	mat2_scaled = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 2.0f, 4.0f));
	mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 41.5f, 161.0f));

	model_matrix = housePos * mat2_translated * initial_rotation * mat2_scaled;

	setTextureMatrix();
	glBindTexture(GL_TEXTURE_2D, houseTexID[1]);
	renderPyramid();

}

/*********************************************************/
/*                     SKY CONTROLLER                    */
/*********************************************************/
void skyController() {
	
	GLfloat disc_scale = 5.0f;

	GLfloat redChange = 0.01f;
	GLfloat greenChange = 0.01f;
	GLfloat blueChange = 0.01f;

	glUseProgram(texture_prog);
	model_matrix = mat4(1.0f);
	mat4 mat2_scaled, mat2_translated, saucer_pos, initial_rotation, other_rot;
	
	initial_rotation *= glm::rotate(mat4(1.0f), radians(9.0f), vec3(1.0f, 0.0f, 0.0f));
	initial_rotation *= glm::rotate(mat4(1.0f), radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	mat4 orbit = glm::translate(mat4(1.0f), vec3(70.0f*cos(radians(-skyAngle)), 40.0f*sin(radians(-skyAngle)),0.0f));

	//sun transformations
	mat2_scaled = glm::scale(glm::mat4(1.0f), glm::vec3(disc_scale, disc_scale, disc_scale));
	mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 36.0f, 100.0f));

	model_matrix =  orbit * mat2_translated * initial_rotation * mat2_scaled;


	setTextureMatrix();
	glBindTexture(GL_TEXTURE_2D, sunTexID);
	renderDisc();
	
	//moon transformations
	mat2_scaled = glm::scale(glm::mat4(1.0f), glm::vec3(disc_scale, disc_scale, disc_scale));
	
	orbit = glm::translate(mat4(1.0f), vec3(70.0f*cos(radians(-skyAngle+180.0f)), 40.0f*sin(radians(-skyAngle + 180.0f)), 0.0f));
	model_matrix = orbit * mat2_translated * initial_rotation * mat2_scaled;

	setTextureMatrix();
	glBindTexture(GL_TEXTURE_2D, moonTexID);
	renderDisc();

	/*vec3 light_intensity(1.0f, 1.0f, 1.0f);
	if (skyAngle < 180)
		light_intensity = vec3(0.7f, 0.7f, 1.0f);

	vec4 light_position(70.0f*cos(radians(-skyAngle)), 40.0f*sin(radians(-skyAngle)), 1.0f, 100.0f);
	glUniform3fv(glGetUniformLocation(texture_prog, "Light.Intensity"), 1, (GLfloat*)&light_intensity);
	glUniform4fv(glGetUniformLocation(texture_prog, "Light.Position"), 1, (GLfloat*)&light_position);*/

	if (skyAngle >= 150.0f && skyAngle <= 180.0f) {
		skyRGB[0] += redChange;
		skyRGB[1] += greenChange;
		skyRGB[2] += blueChange/2;
	}
	else if (skyAngle >= 170.0f && skyAngle <= 260.0f) {
		skyRGB[0] -= 2*redChange;
		skyRGB[1] += greenChange/2;
		skyRGB[2] += blueChange;
	}
	else if (skyAngle < 370.0f && skyAngle > 260.0f) {
		skyRGB[0] += redChange/3;
		skyRGB[1] -= greenChange;
		skyRGB[2] -= blueChange/2;
	}
	else if (skyAngle < 90.0f && skyAngle > 0.0f) {
		skyRGB[0] -= redChange;
		skyRGB[1] -= greenChange;
		skyRGB[2] -= 2.5*blueChange;
	}
	else if (skyAngle == 90.0f) {
		skyRGB[0] = 0.0f;
		skyRGB[1] = 0.0f;
		skyRGB[2] = 0.0f;
	}

	glClearColor(skyRGB[0], skyRGB[1], skyRGB[2], skyRGB[3]);


}

/*********************************************************/
/*                     UFO CONTROLLER                    */
/*********************************************************/
void ufoController() {
	ufo_pos[0] = box_pos[0];
	ufo_pos[1] = box_pos[1];
	ufo_pos[2] = box_pos[2];

	mat4 mat2_scaled, mat2_translated, saucer_pos, rotation;
	saucer_pos = glm::translate(glm::mat4(1.0f), glm::vec3(ufo_pos[0], ufo_pos[1], ufo_pos[2]));
	mat4 end_scale = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 4.0f, 4.0f));

	//pyramid
	model_matrix = mat4(1.0f);
	rotation = glm::rotate(mat4(1.0f), radians(-ufo_rotation), vec3(1.0f, 1.0f, 1.0f));
	mat2_scaled = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.2f, 0.5));
	mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.25, 0.0f));
	model_matrix = mat2_translated * saucer_pos * end_scale * mat2_scaled;

	setTextureMatrix();
	glBindTexture(GL_TEXTURE_2D, ufoTexID[2]);
	renderPyramid();

	//pyramid slightly rotated
	model_matrix = mat4(1.0f);
	mat4 firstrotation = glm::rotate(mat4(1.0f), radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
	rotation = glm::rotate(mat4(1.0f), radians(-ufo_rotation), vec3(1.0f, 1.0f, 1.0f));
	mat2_scaled = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.2f, 0.5));
	mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.25, 0.0f));
	model_matrix = mat2_translated * saucer_pos * firstrotation * end_scale * mat2_scaled;

	setTextureMatrix();
	glBindTexture(GL_TEXTURE_2D, ufoTexID[2]);
	renderPyramid();

	//other box
	model_matrix = mat4(1.0f);
	rotation = glm::rotate(mat4(1.0f), radians(-ufo_rotation), vec3(0.0f, 1.0f, 0.0f));
	mat2_scaled = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.3f, 3.0f));
	mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.70f, 0.0f));
	model_matrix = mat2_translated * saucer_pos * rotation * end_scale * mat2_scaled;

	setTextureMatrix();

	glBindTexture(GL_TEXTURE_2D, ufoTexID[1]);

	renderBox();

	//another box
	model_matrix = mat4(1.0f);
	rotation = glm::rotate(mat4(1.0f), radians(ufo_rotation), vec3(0.0f, 1.0f, 0.0f));
	firstrotation = glm::rotate(mat4(1.0f), radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
	mat2_scaled = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 0.3f, 3.0f));
	//mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(p_pos[0], p_pos[1], 0.0f));
	mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, -0.70f, 0.0f));
	model_matrix = mat2_translated * saucer_pos * firstrotation * rotation * end_scale * mat2_scaled;

	setTextureMatrix();

	glBindTexture(GL_TEXTURE_2D, ufoTexID[0]);

	renderBox();

	//another disc
	model_matrix = mat4(1.0f);
	rotation = glm::rotate(mat4(1.0f), radians(-ufo_rotation), vec3(0.0f, 1.0f, 0.0f));
	firstrotation = glm::rotate(mat4(1.0f), radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
	mat2_scaled = glm::scale(glm::mat4(1.0f), glm::vec3(0.66f, 0.66f, 0.66f));
	mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, -0.5f, 0.0f));
	model_matrix = mat2_translated * saucer_pos * firstrotation * rotation * end_scale * mat2_scaled;

	setTextureMatrix();

	glBindTexture(GL_TEXTURE_2D, ufoTexID[2]);

	renderDisc();

	//ANOTHER disc
	model_matrix = mat4(1.0f);
	rotation = glm::rotate(mat4(1.0f), radians(-ufo_rotation), vec3(0.0f, 1.0f, 0.0f));
	firstrotation = glm::rotate(mat4(1.0f), radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
	mat2_scaled = glm::scale(glm::mat4(1.0f), glm::vec3(0.66f, 0.66f, 0.66f));
	//mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(p_pos[0], p_pos[1], 0.0f));
	mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, -0.9f, 0.0f));
	model_matrix = mat2_translated * saucer_pos * firstrotation * rotation * end_scale * mat2_scaled;

	setTextureMatrix();

	glBindTexture(GL_TEXTURE_2D, ufoTexID[2]);

	renderDisc();

	//TRACTORBEAM
	model_matrix = mat4(1.0f);
	mat2_scaled, mat2_translated;
	rotation = glm::rotate(mat4(1.0f), radians(-ufo_rotation), vec3(0.0f, 5.0f, 0.0f));
	mat2_scaled = glm::scale(glm::mat4(1.0f), glm::vec3(tractor_scale[0], tractor_scale[1], tractor_scale[2]));
	mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -tractor_scale[1]*4, 0.0f));
	model_matrix = mat2_translated * saucer_pos * rotation * end_scale * mat2_scaled;

	setTextureMatrix();
	glBindTexture(GL_TEXTURE_2D, tractorbeamTexID);
	renderCone();

}

/*********************************************************/
/*              TRACTOR BEAM CONTROLLER                  */
/*********************************************************/
void tractorbeamController() {

	//COW COLLISION DETECTOR
	int cow_selector = -1;
	for (int i = 0; i < (sizeof(cowCoordinates) / sizeof(cowCoordinates[0])); i++) {
			if ((ufo_pos[0] >= cowCoordinates[i][0] - 5.0f) &&//checking x 
				(ufo_pos[0] <= cowCoordinates[i][0] + 5.0f) &&
				(ufo_pos[2] >= cowCoordinates[i][2] - 2.0f) &&//checking z
				(ufo_pos[2] <= cowCoordinates[i][2] + 2.0f) &&
				cowCoordinates[i][1] < ufo_pos[1] ) //checking z
			{
				cowLife[i] = false;
				cow_selector = i;
				lastCow = i;
				printf("\nCOW %d TARGETTED", i);
			}
		}

	

	//!!!TRACTOR MODE FUNCTIONS!!!
	//this will modulate the tractor beam to hit the ground and come back up automatically
	if (tractor_mode) {
		tractor_scale[1] += 0.025f;
	}
	else if (tractor_scale[1] > 0.0f) {
		tractor_scale[1] -= 0.025f;

		if (cow_selector > -1)
		{
			cowCoordinates[cow_selector][1] += 0.25;
			cowScale[cow_selector][0] -= 0.1;
			cowScale[cow_selector][1] -= 0.05;
			cowScale[cow_selector][2] -= 0.05;
		}
	}

	if (tractor_scale[1] >= 2.0f) {
		tractor_mode = !tractor_mode;

		if (cow_selector > -1) {
			cowCoordinates[cow_selector][1] = 29.0f;
		}

	}

}

/*********************************************************/
/*                     COW CONTROLLER                    */
/*********************************************************/
void cowController() {

	glUseProgram(texture_prog);
	model_matrix = mat4(1.0f);
	mat4 mat2_scaled, mat2_translated, saucer_pos, initial_rotation, other_rot;

	initial_rotation *= glm::rotate(mat4(1.0f), radians(9.0f), vec3(1.0f, 0.0f, 0.0f));
	//initial_rotation *= glm::rotate(mat4(1.0f), radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	
	//pls change loop parameters to iterate through all cows
	for (int i = 0; i < (sizeof(cowCoordinates) / sizeof(cowCoordinates[0])); i++) {

		if (cowCoordinates[i][1] < ufo_pos[1]) {
			//cow transformations
			mat2_scaled = glm::scale(glm::mat4(1.0f), glm::vec3(cowScale[i][0], cowScale[i][1], cowScale[i][2]));

			mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3((cowCoordinates[i][0]), (cowCoordinates[i][1]), (cowCoordinates[i][2])));
			//mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(box_pos[0], box_pos[1], box_pos[2]));

			model_matrix = mat2_translated * initial_rotation * mat2_scaled;

			setTextureMatrix();
			glBindTexture(GL_TEXTURE_2D, cowTexID[i]);
			renderPlane();
		}

	}
	/*
	//cow transformations
	mat2_scaled = glm::scale(glm::mat4(1.0f), glm::vec3(cowScale[0][0], cowScale[0][1], cowScale[0][2]));
	
	mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3( (cowCoordinates[0][0]), (cowCoordinates[0][1]), (cowCoordinates[0][2]) ));
	//mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(box_pos[0], box_pos[1], box_pos[2]));

	model_matrix =  mat2_translated * initial_rotation * mat2_scaled;

	setTextureMatrix();
	glBindTexture(GL_TEXTURE_2D, cowTexID[0]);
	renderPlane();
*/
}

/*********************************************************/
/*                  WINDMILL CONTROLLER                  */
/*********************************************************/
void windmillController() {
	mat4 windmill_Pos = glm::translate(glm::mat4(1.0f), glm::vec3(windmillPos[0], windmillPos[1], windmillPos[2]));
	//mat4 windmill_Pos = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 35.0f, 180.0f));

	glUseProgram(texture_prog);
	model_matrix = mat4(1.0f);
	mat4 mat2_scaled, mat2_translated, saucer_pos, initial_rotation;
	initial_rotation = glm::rotate(mat4(1.0f), radians(9.0f), vec3(1.0f, 0.0f, 0.0f));
	//initial_rotation *= glm::rotate(mat4(1.0f), radians(25.0f), vec3(0.0f, 1.0f, 0.0f));
	initial_rotation *= glm::rotate(mat4(1.0f), radians(box_rot[0]), vec3(0.0f, 1.0f, 0.0f));


	//base pyramid transformations
	mat2_scaled = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 10.0f, 4.0f));
	mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	model_matrix =  windmill_Pos * mat2_translated * initial_rotation * mat2_scaled;

	setTextureMatrix();
	glBindTexture(GL_TEXTURE_2D, windmillTexID[1]);
	renderPyramid();

	//middle filling box part
	mat2_scaled = glm::scale(glm::mat4(1.0f), glm::vec3(17.0f, 35.0f, 17.0f));
	mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	model_matrix = windmill_Pos * mat2_translated * initial_rotation * mat2_scaled;

	setTextureMatrix();
	glBindTexture(GL_TEXTURE_2D, windmillTexID[0]);
	renderBox();

	//top roof pyramid transformations
	mat2_scaled = glm::scale(glm::mat4(1.0f), glm::vec3(3.5f, 1.50f, 3.5f));
	mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 7.5f, 1.5f));

	model_matrix = windmill_Pos * mat2_translated * initial_rotation * mat2_scaled;

	setTextureMatrix();
	glBindTexture(GL_TEXTURE_2D, windmillTexID[1]);
	renderPyramid();

	//piece that blades connect to
	mat2_scaled = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 10.0f));
	mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.75f, 4.5f));
	initial_rotation *= glm::rotate(mat4(1.0f), radians(windmillAngle), vec3(0.0f, 0.0f, 1.0f));

	model_matrix = windmill_Pos * mat2_translated * initial_rotation * mat2_scaled;

	setTextureMatrix();
	glBindTexture(GL_TEXTURE_2D, windmillTexID[1]);
	renderBox();
	

	//windmill blade one
	model_matrix = mat4(1.0f);
	mat2_scaled = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 3.0f, 0.005f));
	mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.75f, 5.0f));
	glBindTexture(GL_TEXTURE_2D, windmillTexID[1]);
	for (int i = 0; i < 4; i++) {
		
		mat4 orbit = glm::translate(mat4(1.0f), vec3(3.0f*cos(radians(windmillAngle + (i*90))), 3.0f*sin(radians(windmillAngle + (i * 90))), 0.0f));
		initial_rotation = glm::rotate(mat4(1.0f), radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
		initial_rotation *= glm::rotate(mat4(1.0f), radians(windmillAngle + (i * 90)), vec3(0.0f, 0.0f, 1.0f));

		model_matrix = orbit * windmill_Pos  * mat2_translated * initial_rotation * mat2_scaled;

		setTextureMatrix();
		renderPyramid();
	}

}

/*********************************************************/
/*                    TREE CONTROLLER                    */
/*********************************************************/
void treeController() {
	mat4 treePos;
	
	glUseProgram(texture_prog);
	model_matrix = mat4(1.0f);
	mat4 mat2_scaled, mat2_translated, saucer_pos, initial_rotation;
	//initial_rotation = glm::rotate(mat4(1.0f), radians(9.0f), vec3(1.0f, 0.0f, 0.0f));
	

	for (int i = 0; i < (sizeof(treesPos) / sizeof(treesPos[0])); i++) {
		

		treePos = glm::translate(glm::mat4(1.0f), glm::vec3(treesPos[i][0], treesPos[i][1], treesPos[i][2]));
		initial_rotation = glm::rotate(mat4(1.0f), radians(9.0f), vec3(1.0f, 0.0f, 0.0f));

		//tree stump transformation
		mat2_scaled = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f));
		mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

		model_matrix = treePos * mat2_translated * initial_rotation * mat2_scaled;

		setTextureMatrix();
		glBindTexture(GL_TEXTURE_2D, treeTexID[0]);
		renderCone();

		//tree top transformations
		mat2_scaled = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 4.0f, 3.0f));
		mat2_translated = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.5f, 0.5f));
		initial_rotation *= glm::rotate(mat4(1.0f), radians(waves[i]), vec3(0.0f, 0.0f, 1.0f));

		model_matrix = treePos * mat2_translated * initial_rotation * mat2_scaled;

		setTextureMatrix();
		glBindTexture(GL_TEXTURE_2D, treeTexID[1]);
		renderCone();
	}
}

/*********************************************************/
/*                     ON SCREEN TEXT                    */
/*********************************************************/
void onScreenDisplay() {
	unsigned char str[] = "hello world";
	int w;
	w = glutBitmapLength(GLUT_BITMAP_8_BY_13, str);

	glRasterPos2f(-0.9, 0.9);

	glColor3f(1.0f, 0.0f, 0.0f);

	int len = sizeof(str) / sizeof(char);

	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str[i]);
	}
}

/**************************************************************************************************************/
void Display(void){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	grassGroundController();//controls ground and grass
	houseController();// controls house
	skyController(); // controls sun, moon, and sky color
	tractorbeamController(); // controls tractorbeam and cow effects
	ufoController(); // controls the ufo
	cowController(); // controls the cows
	windmillController(); // controls windmill
	treeController(); // controls trees

	//onScreenDisplay();

	glutSwapBuffers();

}
/**************************************************************************************************************/


void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'a':
		box_pos[0] -= 0.25f;
		printf("\nbox_pos[0] = %f", box_pos[0]);
		break;
	case 'd':
		box_pos[0] += 0.25f;
		printf("\nbox_pos[0] = %f", box_pos[0]);
		break;
	case 'w':
		box_pos[2] -= 0.25f;
		printf("\nbox_pos[2] = %f", box_pos[2]);
		break;
	case 's':
		box_pos[2] += 0.25f;
		printf("\nbox_pos[2] = %f", box_pos[2]);
		break;
	case 'e':
		windmillSpeed += 1.0f;
		printf("\nwindmillSpeed = %f", windmillSpeed);
		break;
	case 'r':
		windmillSpeed -= 1.0f;
		printf("\nwindmillSpeed = %f", windmillSpeed);
		break;
	case 'f':
		box_scale[0] += 1.0f;
		box_scale[1] += 1.0f;
		box_scale[2] += 1.0f;
		printf("\nbox_scale[1] = %f", box_scale[1]);
		break;
	case 'c':
		box_scale[0] -= 1.0f;
		box_scale[1] -= 1.0f;
		box_scale[2] -= 1.0f;
		printf("\nbox_scale[1] = %f", box_scale[1]);
		break;
	case 't':
		tractor_mode = !tractor_mode;
		printf("\ntractor_mode", box_rot[0]);
		break;
	case 'g':
		box_rot[0] -= 1.0f;
		printf("\nbox_rot[0] = %f", box_rot[0]);
		break;
	case 'v':
		box_rot[0] += 1.0f;
		printf("\nbox_rot[0] = %f", box_rot[0]);
		break;
	case 'z':
		skyAngle_add += 0.025f;
		printf("\nskyAngle_add = %f",skyAngle_add);
		break;
	case 'x':
		skyAngle_add -= 0.025f;
		printf("\nskyAngle_add = %f", skyAngle_add);
		break;
	case 'y':
		//resets cows
		//box_pos[0] = 0.0f;
		//box_pos[1] = 0.0f;
		//box_pos[2] = 0.0f;
		cowSetter();
		break;

	}
}


/*************************************************************************/
void Rotate(int n)  //the "glutTimerFunc"
{
	if (n == 0) {
		skyAngle = fmod((skyAngle + skyAngle_add), 360.0f);
		ufo_rotation += 1.0f;
		windmillAngle += windmillSpeed;
		glutTimerFunc(20, Rotate, 0);
	}
	else if (n == 1) {
		for (int i = 0; i < (sizeof(treesPos) / sizeof(treesPos[0])); i++) {
			if (waves[i] > 3.0f || waves[i] < -3.0f) {
				treeWaves[i] = !treeWaves[i];
				
			}

			if (treeWaves[i])
				waves[i] += 0.1f;
			else
				waves[i] -= 0.1f;
		}

		glutTimerFunc(30,Rotate, 1);
	}

	glutPostRedisplay();
}

/************************************************************************/
int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(1024, 1024);

	glutCreateWindow("Author: Erik Almaguer      CONTROLS - MOVE: WASD   TRACTOR BEAM: T    RESET COWS: Y    +WINDMILL SPEED: E    -WINDMILL SPEED: R");

	if (glewInit()) {
		printf("Unable to initialize GLEW ... exiting\n");
	}
	ilInit();
	Initialize();
	/*printf("\ncowCoordinates[0][0] = %f", cowCoordinates[0][0]);
	printf("\ncowCoordinates[0][1] = %f", cowCoordinates[0][1]);
	printf("\ncowCoordinates[0][2] = %f\n", cowCoordinates[0][2]);*/
	printf("%s\n", glGetString(GL_VERSION));
	glutDisplayFunc(Display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(Reshape);
	glutTimerFunc(20, Rotate, 0);
	glutTimerFunc(30, Rotate, 1);
	glutMainLoop();
	return 0;
}

/*****/



/*************/