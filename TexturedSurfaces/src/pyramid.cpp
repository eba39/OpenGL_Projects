#include "pyramid.h"

unsigned int pyramid_vao;

void createPyramid()
{
	// position of the vertices in the rectangle

	GLfloat vertices[] = {  //front face
							0.0f, 1.0f, 0.0, 1.0, // 0
	                        -0.75,  -0.75, 0.75, 1.0,//1
	                        0.75, -0.75, 0.75, 1.0,   //2

							//right face
							0.0f, 1.0f, 0.0, 1.0,
							0.75,  -0.75, 0.75, 1.0,
							0.75, -0.75, -0.75, 1.0, 

							
							//rear face
							0.0f, 1.0f, 0.0, 1.0,
							0.75,  -0.75, -0.75, 1.0,
							-0.75, -0.75, -0.75, 1.0,

							//left face
							0.0f, 1.0f, 0.0, 1.0,
							-0.75,  -0.75, -0.75, 1.0,
							-0.75, -0.75, 0.75, 1.0,

							//bottom face
							-0.75f, -0.75f, 0.75f, 1.0f,
							0.75f, -0.75f, 0.75f, 1.0f,
							0.75f, -0.75f, -0.75f, 1.0f,
							-0.75f, -0.75f, -0.75f, 1.0f,
	                     };

	// Indices for the hexagon strips
	const GLushort indices[] =
	{
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11,
		12, 13, 14,
		12, 14, 15
		
	};

	float pyramid_normals[] = {
		//front trianlge of pyramid
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		
		// Right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,


		//Back
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		//left
		-1.0f, 0.0f , 0.0f,
		-1.0f, 0.0f , 0.0f,
		-1.0f, 0.0f , 0.0f,

		// bottom square
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,

		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,

	};

	float pyramid_textures[] = {
		//front
		0.5f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		//right
		0.5f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		//back
		0.5f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		//left 
		0.5f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		//bottom
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,

		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
	};

	glGenVertexArrays(1, &pyramid_vao);
	glBindVertexArray(pyramid_vao);

	unsigned int handle[4];
	glGenBuffers(4, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // Vertex position

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_normals), pyramid_normals, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ARRAY_BUFFER, 18 * 2 * sizeof(float), pyramid_textures, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0,0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);



}

void renderPyramid() {
	glBindVertexArray(pyramid_vao);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);
}


