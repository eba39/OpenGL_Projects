#include "disc.h"

GLuint disc_vao;
//GLuint box_vbo;
//GLuint box_ebo;

GLuint disc_indices[NumDiscIndices];
vec4 disc_points[NumDiscPoints + 1];
vec3 disc_normals[NumDiscPoints + 1];
vec2 disc_textures[NumDiscPoints + 1];

int disc_index = 0;
static const double disc_kPI = 3.1415926535897932384626433832795;

void createDisc()
{
	disc_points[disc_index][0] = 0.0;
	disc_points[disc_index][1] = 0.0;
	disc_points[disc_index][2] = 0.0;
	disc_points[disc_index][3] = 0.0;

	disc_normals[disc_index][0] = 0.0;
	disc_normals[disc_index][1] = 0.0;
	disc_normals[disc_index][2] = 0.0;

	disc_textures[disc_index][0] = 0.5f;
	disc_textures[disc_index][1] = 0.5f;

	disc_index++;

	int i;
	float theta;
	int tIndices = 0;


	for (i = 0; i < NumDiscPoints; ++i) 
	{

		theta = i * 20.0f*disc_kPI / 180.0f;

		disc_points[disc_index][0] = cos(theta);
		disc_points[disc_index][1] = 0.0;
		disc_points[disc_index][2] = -sin(theta);
		disc_points[disc_index][3] = 0.0;

		disc_normals[disc_index][0] = 0.0;
		disc_normals[disc_index][1] = 0.0;
		disc_normals[disc_index][2] = 0.0;

		disc_textures[disc_index][0] = (1 + cos(theta))/2;
		disc_textures[disc_index][1] = (1 + sin(theta))/2;

		if (i <= (NumDiscPoints - 2)) {

			disc_indices[tIndices] = 0; tIndices++;
			disc_indices[tIndices] = disc_index; tIndices++;
			disc_indices[tIndices] = disc_index + 1; tIndices++;

		}
		else {
			disc_indices[tIndices] = 0; tIndices++;
			disc_indices[tIndices] = disc_index; tIndices++;
			disc_indices[tIndices] = 1; tIndices++;
		}
		disc_index++;
	}

	updateDiscVertexNormals();




	glGenVertexArrays(1, &disc_vao);
	glBindVertexArray(disc_vao);

	unsigned int handle[4];
	glGenBuffers(4, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(disc_points), disc_points, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // Vertex position

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(disc_normals), disc_normals, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);  // Vertex normal

	glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(disc_textures), disc_textures, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);  // texture coords

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumDiscIndices * sizeof(GLuint), disc_indices, GL_STATIC_DRAW);


	glBindVertexArray(0);

}

void updateDiscVertexNormals() {
	vec4 P1, P2, P3;
	vec3 N;
	int i;// Calculate vertex normals
	for (i = 0; i < NumDiscIndices; i += 3) {
		P1 = disc_points[disc_indices[i + 0]];
		P2 = disc_points[disc_indices[i + 1]];
		P3 = disc_points[disc_indices[i + 2]];
		N = normalize(cross(vec3(P2 - P1), (vec3(P3 - P1))));
		disc_normals[disc_indices[i + 0]] += N;
		disc_normals[disc_indices[i + 1]] += N;
		disc_normals[disc_indices[i + 2]] += N;
	}
	//normalize normalsfor(i = 0; i < NumdiscPoints; i++)normals[i] =normalize(vec3(normals[i]));
}

void renderDisc() {
	glBindVertexArray(disc_vao);
	//glDrawArrays(GL_TRIANGLES, 0, 18);
	glDrawElements(GL_TRIANGLES, NumDiscIndices, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}


