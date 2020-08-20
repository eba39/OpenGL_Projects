#include "hexagon.h"

GLuint Hexagon_vao;

GLuint Hexagon_indices[NumHexagonIndices];
vec4 Hexagon_points[NumHexagonPoints + 1];
vec3 Hexagon_normals[NumHexagonPoints + 1];
vec2 Hexagon_textures[NumHexagonPoints + 1];

int Hexagon_index = 0;
static const double Hexagon_kPI = 3.1415926535897932384626433832795;

void createHexagon()
{
	Hexagon_points[Hexagon_index][0] = 0.0;
	Hexagon_points[Hexagon_index][1] = 0.0;
	Hexagon_points[Hexagon_index][2] = 0.0;
	Hexagon_points[Hexagon_index][3] = 1.0f;

	Hexagon_normals[Hexagon_index][0] = 0.0;
	Hexagon_normals[Hexagon_index][1] = 0.0;
	Hexagon_normals[Hexagon_index][2] = 0.0;

	Hexagon_textures[Hexagon_index][0] = 0.5f;
	Hexagon_textures[Hexagon_index][1] = 0.5f;

	Hexagon_index++;

	int i;
	float theta;
	int tIndices = 0;


	for (i = 0; i < NumHexagonPoints; ++i) 
	{

		theta = i * 60.0f*Hexagon_kPI / 180.0f;

		Hexagon_points[Hexagon_index][0] = cos(theta);
		Hexagon_points[Hexagon_index][1] = -sin(theta);
		Hexagon_points[Hexagon_index][2] = 0.0f;
		Hexagon_points[Hexagon_index][3] = 1.0f;

		Hexagon_normals[Hexagon_index][0] = 0.0;
		Hexagon_normals[Hexagon_index][1] = 0.0;
		Hexagon_normals[Hexagon_index][2] = 0.0;

		Hexagon_textures[Hexagon_index][0] = (1 + cos(theta))/2;
		Hexagon_textures[Hexagon_index][1] = (1 + sin(theta))/2;

		if (i <= (NumHexagonPoints - 2)) {

			Hexagon_indices[tIndices] = 0; 
			tIndices++;
			Hexagon_indices[tIndices] = Hexagon_index; 
			tIndices++;
			Hexagon_indices[tIndices] = Hexagon_index + 1; 
			tIndices++;

		}
		else {
			Hexagon_indices[tIndices] = 0; tIndices++;
			Hexagon_indices[tIndices] = Hexagon_index; tIndices++;
			Hexagon_indices[tIndices] = 1; tIndices++;
		}
		Hexagon_index++;
	}

	updateHexagonVertexNormals();


	glGenVertexArrays(1, &Hexagon_vao);
	glBindVertexArray(Hexagon_vao);

	unsigned int handle[4];
	glGenBuffers(4, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Hexagon_points), Hexagon_points, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // Vertex position

	
	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Hexagon_normals), Hexagon_normals, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);  // Vertex normal

	
	glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Hexagon_textures), Hexagon_textures, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);  // texture coords
	
	

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumHexagonIndices * sizeof(GLuint), Hexagon_indices, GL_STATIC_DRAW);


	glBindVertexArray(0);

}

void updateHexagonVertexNormals() {
	vec4 P1, P2, P3;
	vec3 N;
	int i;// Calculate vertex normals
	for (i = 0; i < NumHexagonIndices; i += 3) {
		P1 = Hexagon_points[Hexagon_indices[i + 0]];
		P2 = Hexagon_points[Hexagon_indices[i + 1]];
		P3 = Hexagon_points[Hexagon_indices[i + 2]];
		N = normalize(cross(vec3(P2 - P1), (vec3(P3 - P1))));
		Hexagon_normals[Hexagon_indices[i + 0]] += N;
		Hexagon_normals[Hexagon_indices[i + 1]] += N;
		Hexagon_normals[Hexagon_indices[i + 2]] += N;
	}
	//normalize normals
	for (i = 0; i < NumHexagonPoints; i++)Hexagon_normals[i] = normalize(vec3(Hexagon_normals[i]));
}

void renderHexagon() {
	glBindVertexArray(Hexagon_vao);
	glDrawElements(GL_TRIANGLES, NumHexagonIndices, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}


