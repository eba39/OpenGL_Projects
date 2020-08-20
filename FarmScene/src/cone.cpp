#include "cone.h"

GLuint cone_vao;
//GLuint box_vbo;
//GLuint box_ebo;

GLuint cone_indices[NumIndices];
vec4 cone_points[NumConePoints + 1];
vec3 cone_normals[NumConePoints + 1];
vec2 cone_textures[NumConePoints + 1];

int cone_index = 0;
static const double kPI = 3.1415926535897932384626433832795;

void createCone()
{
	cone_points[cone_index][0] = 0.0;
	cone_points[cone_index][1] = 1.0;
	cone_points[cone_index][2] = 0.0;
	cone_points[cone_index][3] = 1.0;

	cone_normals[cone_index][0] = 0.0;
	cone_normals[cone_index][1] = 0.0;
	cone_normals[cone_index][2] = 0.0;

	cone_textures[cone_index][0] = 0.5f;
	cone_textures[cone_index][1] = 0.5f;

	cone_index++;

	int i;
	float theta;
	int tIndices = 0;


	for (i = 0; i < NumConePoints; ++i) 
	{

		theta = i * 20.0f*kPI / 180.0f;

		cone_points[cone_index][0] = cos(theta);
		cone_points[cone_index][1] = -1.0;
		cone_points[cone_index][2] = -sin(theta);
		cone_points[cone_index][3] = 1.0;

		cone_normals[cone_index][0] = 0.0;
		cone_normals[cone_index][1] = 0.0;
		cone_normals[cone_index][2] = 0.0;

		cone_textures[cone_index][0] = (1 + cos(theta)) /2;
		cone_textures[cone_index][1] = (1 + sin(theta)) /2;

		if (i <= (NumConePoints - 2)) {

			cone_indices[tIndices] = 0; tIndices++;
			cone_indices[tIndices] = cone_index; tIndices++;
			cone_indices[tIndices] = cone_index + 1; tIndices++;

		}
		else {
			cone_indices[tIndices] = 0; tIndices++;
			cone_indices[tIndices] = cone_index; tIndices++;
			cone_indices[tIndices] = 1; tIndices++;
		}
		cone_index++;
	}

	updateVertexNormals();




	glGenVertexArrays(1, &cone_vao);
	glBindVertexArray(cone_vao);

	unsigned int handle[4];
	glGenBuffers(4, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cone_points), cone_points, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // Vertex position

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cone_normals), cone_normals, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);  // Vertex normal

	glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cone_textures), cone_textures, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);  // texture coords

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumIndices * sizeof(GLuint), cone_indices, GL_STATIC_DRAW);


	glBindVertexArray(0);

}

void updateVertexNormals() {
	vec4 P1, P2, P3;
	vec3 N;
	int i;// Calculate vertex normals
	for (i = 0; i < NumIndices; i += 3) {
		P1 = cone_points[cone_indices[i + 0]];
		P2 = cone_points[cone_indices[i + 1]];
		P3 = cone_points[cone_indices[i + 2]];
		N = normalize(cross(vec3(P2 - P1), (vec3(P3 - P1))));
		cone_normals[cone_indices[i + 0]] += N;
		cone_normals[cone_indices[i + 1]] += N;
		cone_normals[cone_indices[i + 2]] += N;
	}
	//normalize normalsfor(i = 0; i < NumConePoints; i++)normals[i] =normalize(vec3(normals[i]));
}

void renderCone() {
	glBindVertexArray(cone_vao);
	//glDrawArrays(GL_TRIANGLES, 0, 18);
	glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}


