#pragma once

#include <GL/gl.h>
#include "lext.h"

GLuint vertexbuffer;
GLuint indexbuffer;

// https://www.khronos.org/opengl/wiki/VBO_-_just_examples
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/

float g_vertex_buffer_data[8][3] =
{
	{ -0.25f, 0.25f, -0.25f },
	{ 0.25f, 0.25f, -0.25f },
	{ -0.25f, -0.25f, -0.25f },
	{ 0.25f, -0.25f, -0.25f },
	{ -0.25f, 0.25f, 0.25f },
	{ 0.25f, 0.25f, 0.25f },
	{ -0.25f, -0.25f, 0.25f },
	{ 0.25f, -0.25f, 0.25f }
};

GLuint g_index_buffer_data[] =
{
	0, 1, 2,    // side 1
	2, 1, 3,
	4, 0, 6,    // side 2
	6, 0, 2,
	7, 5, 6,    // side 3
	6, 5, 4,
	3, 1, 7,    // side 4
	7, 1, 5,
	4, 5, 0,    // side 5
	0, 5, 1,
	3, 7, 2,    // side 6
	2, 7, 6,
};

void l_create_vertex_array()
{
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data),
		g_vertex_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &indexbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_index_buffer_data),
		g_index_buffer_data, GL_STATIC_DRAW);

}

void l_draw_plane()
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(float) * 3, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);

	glEnableClientState(GL_VERTEX_ARRAY);
}