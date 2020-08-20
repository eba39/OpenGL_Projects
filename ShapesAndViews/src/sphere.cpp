#include "sphere.h"

GLuint Sphere_vao;

GLuint Sphere_indices[NumSphereIndices];
vec4 Sphere_points[NumSpherePoints];
vec3 Sphere_normals[NumSpherePoints + 2];
vec2 Sphere_textures[NumSpherePoints + 2];

int Sphere_index = 0;
static const double kPI = 3.1415926535897932384626433832795;

void createSphere()
{
	
	float theta;
	float phi;
	int i = 0;
	for (double b = -STEP / 2.0; b <= STEP / 2.0; b++) {
		if (b == -STEP / 2.0 || b == STEP / 2.0) {
			theta = (1.0 * b / STEP) *kPI;
			Sphere_points[i].x = 0;
			Sphere_points[i].y = sin(theta);
			Sphere_points[i].z = 0;
			Sphere_points[i].w = 1.0f;
			//printf("\nSphere_points[%d] = [%f, %f, %f]\n",i, Sphere_points[i].x, Sphere_points[i].y, Sphere_points[i].z);
			i++;
		}
		else {
			for (int a = 0; a < STEP; a++) {
				// lat/lon coordinates
				phi = (1.0 * a / STEP) * 2 * kPI;
				theta = (1.0 * b / STEP) *kPI;
				Sphere_points[i].x = cos(theta)*sin(phi);
				Sphere_points[i].y = sin(theta);
				Sphere_points[i].z = cos(theta)*cos(phi);
				Sphere_points[i].w = 1.0f;
				//printf("\nSphere_points[%d] = (%f, %f, %f)",i, Sphere_points[i].x, Sphere_points[i].y, Sphere_points[i].z);
				i++;
			}
		}
	}

	//constructing bottom
	Sphere_index = 0;
	for (i = 1; i < STEP+1; i++) {
		if (i <= STEP - 1) {
			Sphere_indices[Sphere_index] = 0;
			Sphere_index++;
			Sphere_indices[Sphere_index] = i;
			Sphere_index++;
			Sphere_indices[Sphere_index] = i+1;
			Sphere_index++;
		}
		else {
			Sphere_indices[Sphere_index] = 0;
			Sphere_index++;
			Sphere_indices[Sphere_index] = STEP;
			Sphere_index++;
			Sphere_indices[Sphere_index] = 1;
			Sphere_index++;
		}
	}

	//middle indices
	for (int i = 1; i < NumSpherePoints - STEP - 1; i+=STEP) {

		for (int j = i; j < (i + STEP); j++) {
			if (j == i + STEP - 1) {
				Sphere_indices[Sphere_index] = j;
				Sphere_index++;
				Sphere_indices[Sphere_index] = j + STEP;
				Sphere_index++;
				Sphere_indices[Sphere_index] = i;
				Sphere_index++;

				Sphere_indices[Sphere_index] = j + STEP;
				Sphere_index++;
				Sphere_indices[Sphere_index] = i + STEP;
				Sphere_index++;
				Sphere_indices[Sphere_index] = i;
				Sphere_index++;
			}
			else {
				Sphere_indices[Sphere_index] = j;
				Sphere_index++;
				Sphere_indices[Sphere_index] = j + STEP;
				Sphere_index++;
				Sphere_indices[Sphere_index] = j + 1;
				Sphere_index++;

				Sphere_indices[Sphere_index] = j + STEP;
				Sphere_index++;
				Sphere_indices[Sphere_index] = j + STEP + 1;
				Sphere_index++;
				Sphere_indices[Sphere_index] = j + 1;
				Sphere_index++;
			}
		}
	}


	//top indices
	//printf("\n\nlast indices");

	for (i = NumSpherePoints-STEP-1; i < NumSpherePoints-1; i++) {
		if (i <= NumSpherePoints - 3) {
			Sphere_indices[Sphere_index] = NumSpherePoints - 1;
			Sphere_index++;
			Sphere_indices[Sphere_index] = i + 1;
			Sphere_index++;
			Sphere_indices[Sphere_index] = i;
			Sphere_index++;
			//printf("\nBUILD: %d", i);
		}
		else {
			Sphere_indices[Sphere_index] = NumSpherePoints - 1;
			Sphere_index++;
			Sphere_indices[Sphere_index] = NumSpherePoints - STEP - 1;
			Sphere_index++;
			Sphere_indices[Sphere_index] = i;
			Sphere_index++;
			//printf("\nLAST: %d",i);
		}
		printf("\nSphere_indices[%d]= %d",Sphere_index-3, Sphere_indices[Sphere_index-3]);
		//printf("\nSphere_indices[%d]= %d", Sphere_index - 2, Sphere_indices[Sphere_index - 2]);
		//printf("\nSphere_indices[%d]= %d\n", Sphere_index - 1, Sphere_indices[Sphere_index - 1]);
	}
	//to get normals
	//normalized vec3 of each vertex


	updateSphereVertexNormals();


	glGenVertexArrays(1, &Sphere_vao);
	glBindVertexArray(Sphere_vao);

	unsigned int handle[4];
	glGenBuffers(4, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Sphere_points), Sphere_points, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // Vertex position

	
	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Sphere_normals), Sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);  // Vertex normal

	/*
	glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Sphere_textures), Sphere_textures, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);  // texture coords
	*/

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumSphereIndices * sizeof(GLuint), Sphere_indices, GL_STATIC_DRAW);


	glBindVertexArray(0);

}

void updateSphereVertexNormals() {
	vec4 P1, P2, P3;
	vec3 N;
	int i;// Calculate vertex normals
	for (i = 0; i < NumSphereIndices; i += 3) {
		P1 = Sphere_points[Sphere_indices[i + 0]];
		P2 = Sphere_points[Sphere_indices[i + 1]];
		P3 = Sphere_points[Sphere_indices[i + 2]];
		N = normalize(cross(vec3(P2 - P1), (vec3(P3 - P1))));
		Sphere_normals[Sphere_indices[i + 0]] += N;
		Sphere_normals[Sphere_indices[i + 1]] += N;
		Sphere_normals[Sphere_indices[i + 2]] += N;
	}
	//normalize normals
	for (i = 0; i < NumSpherePoints; i++)Sphere_normals[i] = normalize(vec3(-Sphere_normals[i]));
}

void renderSphere() {
	glBindVertexArray(Sphere_vao);
	glDrawElements(GL_TRIANGLES, NumSphereIndices, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}


