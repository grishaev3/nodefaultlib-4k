#pragma once

#include <GL/gl.h>
#include "lshader.h"

GLuint vertexbuffer;
GLuint indexbuffer;

// https://www.khronos.org/opengl/wiki/VBO_-_just_examples
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/

float g_vertex_buffer_data[24][3] =
{
	{0.25f, 0.25f,-0.25f},
	{0.25f,-0.25f,-0.25f},
	{-0.25f,-0.25f,-0.25f},
	{-0.25f, 0.25f,-0.25f},
	{0.25f, 0.25f, 0.25f},
	{-0.25f, 0.25f, 0.25f},
	{-0.25f,-0.25f, 0.25f},
	{0.25f,-0.25f, 0.25f},
	{0.25f, 0.25f,-0.25f},
	{0.25f, 0.25f, 0.25f},
	{0.25f,-0.25f, 0.25f},
	{0.25f,-0.25f,-0.25f},
	{0.25f,-0.25f,-0.25f},
	{0.25f,-0.25f, 0.25f},
	{-0.25f,-0.25f, 0.25f},
	{-0.25f,-0.25f,-0.25f},
	{-0.25f,-0.25f,-0.25f},
	{-0.25f,-0.25f, 0.25f},
	{-0.25f, 0.25f, 0.25f},
	{-0.25f, 0.25f,-0.25f},
	{0.25f, 0.25f, 0.25f},
	{0.25f, 0.25f,-0.25f},
	{-0.25f, 0.25f,-0.25f},
	{-0.25f, 0.25f, 0.25f},
};

GLuint g_index_buffer_data[36] = {
	0,1,2,2,3,0,
	4,5,6,6,7,4,
	8,9,10,10,11,8,
	12,13,14,14,15,12,
	16,17,18,18,19,16,
	20,21,22,22,23,20
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
	glVertexPointer(3, GL_FLOAT, sizeof(float)*3, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);

	glEnableClientState(GL_VERTEX_ARRAY);
}