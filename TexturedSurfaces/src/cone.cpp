#include "cone.h"

GLuint Cone_vao;

GLuint Cone_indices[NumConeIndices];
vec4 Cone_points[NumConePoints + 1];
vec3 Cone_normals[NumConePoints + 1];
vec2 Cone_textures[NumConePoints + 1];

int Cone_index = 0;
static const double Cone_kPI = 3.1415926535897932384626433832795;

void createCone()
{
	Cone_points[Cone_index][0] = 0.0;
	Cone_points[Cone_index][1] = 0.0;
	Cone_points[Cone_index][2] = -1.0;
	Cone_points[Cone_index][3] = 1.0f;

	Cone_normals[Cone_index][0] = 0.0;
	Cone_normals[Cone_index][1] = 0.0;
	Cone_normals[Cone_index][2] = 0.0;

	Cone_textures[Cone_index][0] = 0.5f;
	Cone_textures[Cone_index][1] = 0.5f;

	Cone_index++;

	int i;
	float theta;
	int tIndices = 0;


	for (i = 0; i < NumConePoints; ++i) 
	{

		theta = i * 20.0f*Cone_kPI / 180.0f;

		Cone_points[Cone_index][0] = cos(theta);
		Cone_points[Cone_index][1] = -sin(theta);
		Cone_points[Cone_index][2] = 1.0f;
		Cone_points[Cone_index][3] = 1.0f;

		Cone_normals[Cone_index][0] = 0.0;
		Cone_normals[Cone_index][1] = 0.0;
		Cone_normals[Cone_index][2] = 0.0;

		Cone_textures[Cone_index][0] = (1 + cos(theta))/2;
		Cone_textures[Cone_index][1] = (1 + sin(theta))/2;

		if (i <= (NumConePoints - 2)) {

			Cone_indices[tIndices] = 0; 
			tIndices++;
			Cone_indices[tIndices] = Cone_index; 
			tIndices++;
			Cone_indices[tIndices] = Cone_index + 1; 
			tIndices++;

		}
		else {
			Cone_indices[tIndices] = 0; tIndices++;
			Cone_indices[tIndices] = Cone_index; tIndices++;
			Cone_indices[tIndices] = 1; tIndices++;
		}
		Cone_index++;
	}

	updateConeVertexNormals();


	glGenVertexArrays(1, &Cone_vao);
	glBindVertexArray(Cone_vao);

	unsigned int handle[4];
	glGenBuffers(4, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Cone_points), Cone_points, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // Vertex position

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Cone_normals), Cone_normals, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);  // Vertex normal

	glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Cone_textures), Cone_textures, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);  // texture coords

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumConeIndices * sizeof(GLuint), Cone_indices, GL_STATIC_DRAW);


	glBindVertexArray(0);

}

void updateConeVertexNormals() {
	vec4 P1, P2, P3;
	vec3 N;
	int i;// Calculate vertex normals
	for (i = 0; i < NumConeIndices; i += 3) {
		P1 = Cone_points[Cone_indices[i + 0]];
		P2 = Cone_points[Cone_indices[i + 1]];
		P3 = Cone_points[Cone_indices[i + 2]];
		N = normalize(cross(vec3(P2 - P1), (vec3(P3 - P1))));
		Cone_normals[Cone_indices[i + 0]] += N;
		Cone_normals[Cone_indices[i + 1]] += N;
		Cone_normals[Cone_indices[i + 2]] += N;
	}
	//normalize normals

	for (i = 0; i < NumConePoints; i++)Cone_normals[i] = normalize(vec3(Cone_normals[i]));
}

void renderCone() {
	glBindVertexArray(Cone_vao);
	glDrawElements(GL_TRIANGLES, NumConeIndices, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}


