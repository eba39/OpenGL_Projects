#include "Triangle.h"

unsigned int tri_vao;

void createTriangle()
{
	// position of the vertices in the rectangle

	GLfloat vertices[] = {  -0.90, -0.90, 0.0, 1.0, // bottom_left
	                        0.90,  -0.90, 0.0, 1.0, // bottom_right
	                        0.90, 0.90, 0.0, 1.0 ,   // top_right
	                      
	                     };

	// Indices for the hexagon strips
	const GLushort indices[] =
	{
		0, 1, 2,  // Triangle consisting of three indices: '0', '1' and '2'
		
	};

	GLfloat normals[] = {
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f
	};

	glGenVertexArrays(1, &tri_vao);
	glBindVertexArray(tri_vao);

	unsigned int handle[3];
	glGenBuffers(3, handle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 4 * sizeof(float), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // Vertex position

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(GLushort), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);



}

void renderTriangle() {
	glBindVertexArray(tri_vao);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
}


