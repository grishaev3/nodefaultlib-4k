#pragma once

#include <GL/gl.h>
#include "lext.h"

GLuint vertexbuffer;
GLuint indexbuffer;
GLuint normalbuffer;

// https://www.khronos.org/opengl/wiki/VBO_-_just_examples
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/

float g_vertex_buffer_data[8][3] =
{
	{0.25f,0.25f,-0.25f},
	{0.25f,-0.25f,-0.25f},
	{-0.25f,-0.25f,-0.25f},
	{-0.25f,0.25f,-0.25f},
	{0.25f,0.25f,0.25f},
	{0.25f,-0.25f,0.25f},
	{-0.25f,-0.25f,0.25f},
	{-0.25f,0.25f,0.25f} ,
};

GLuint g_index_buffer_data[] =
{
	0,1,2,
	3,0,2,
	4,7,6,
	5,4,6,
	0,4,5,
	1,0,5,
	1,5,6,
	2,1,6,
	2,6,7,
	3,2,7,
	4,0,3,
	7,4,3
};


vec3_t n_normal_buffer_data[24] = {
	{0.000000,0.000000,-1.000000},
	{0.000000,0.000000,-1.000000},
	{0.000000,0.000000,-1.000000},
	{0.000000,0.000000,-1.000000},
	{0.000000,0.000000,1.000000},
	{0.000000,0.000000,1.000000},
	{0.000000,0.000000,1.000000},
	{0.000000,0.000000,1.000000},
	{1.000000,-0.000000,0.000000},
	{1.000000,-0.000000,0.000000},
	{1.000000,-0.000000,0.000000},
	{1.000000,-0.000000,0.000000},
	{-0.000000,-1.000000,-0.000000},
	{-0.000000,-1.000000,-0.000000},
	{-0.000000,-1.000000,-0.000000},
	{-0.000000,-1.000000,-0.000000},
	{-1.000000,0.000000,-0.000000},
	{-1.000000,0.000000,-0.000000},
	{-1.000000,0.000000,-0.000000},
	{-1.000000,0.000000,-0.000000},
	{0.000000,1.000000,0.000000},
	{0.000000,1.000000,0.000000},
	{0.000000,1.000000,0.000000},
	{0.000000,1.000000,0.000000}
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

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(n_normal_buffer_data),
		n_normal_buffer_data, GL_STATIC_DRAW);
}

void l_draw_plane()
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(float) * 3, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, sizeof(float) * 3, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	glDrawElements(GL_TRIANGLES, sizeof(g_index_buffer_data) /
		sizeof(g_index_buffer_data[0]), GL_UNSIGNED_INT, (void*)0);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

#ifdef IS_STATIC_NORMALS
void l_calculate_normals()
{
	for (uint32_t i = 0; i < sizeof(g_index_buffer_data) /
		sizeof(g_index_buffer_data[0]) / 3; i++) {

		uint32_t index0 = g_index_buffer_data[i * 3 + 0];
		uint32_t index1 = g_index_buffer_data[i * 3 + 1];
		uint32_t index2 = g_index_buffer_data[i * 3 + 2];

		vec3_t *a = (vec3_t*) { g_vertex_buffer_data[index0] };
		vec3_t *b = (vec3_t*) { g_vertex_buffer_data[index1] };
		vec3_t *c = (vec3_t*) { g_vertex_buffer_data[index2] };

		vec3_t dir = v3_norm(v3_cross(v3_sub(*b, *a), v3_sub(*c, *a)));
		vec3_t ndir = (vec3_t) { -dir.x, -dir.y, -dir.z };


		n_normal_buffer_data[index0] =
			n_normal_buffer_data[index1] =
			n_normal_buffer_data[index2] = ndir;
		/*
		vec3_t dir = Vector3.Cross(b - a, c - a);
		vec3_t norm = Vector3.Normalize(dir);*/
	}
}
#endif // !IS_STATIC_NORMALS