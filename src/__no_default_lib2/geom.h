#pragma once

#include <GL/gl.h>
#include "lmath.h"
#include "math_3d.h"

float ver[8][3] =
{
	{ -0.25,-0.25,0.25 },
	{ -0.25,0.25,0.25 },
	{ 0.25,0.25,0.25 },
	{ 0.25,-0.25,0.25 },
	{ -0.25,-0.25,-0.25 },
	{ -0.25,0.25,-0.25 },
	{ 0.25,0.25,-0.25 },
	{ 0.25,-0.25,-0.25 },
};

void quad(int a, int b, int c, int d)
{
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);

	glVertex3fv(ver[a]);
	glVertex3fv(ver[b]);
	glVertex3fv(ver[c]);
	glVertex3fv(ver[d]);
	glEnd();
}

void draw_plane()
{
	quad(0, 3, 2, 1);
	quad(2, 3, 7, 6);
	quad(0, 4, 7, 3);
	quad(1, 2, 6, 5);
	quad(4, 5, 6, 7);
	quad(0, 1, 5, 4);
}

typedef struct {
	//numberOfBlock = 3;	// кол-во блоков в окружности
	//windingCount = 1;		// кол-во слоёв (мотков) с увеличивающимся радиусом
	//segCount=windingCount*numberOfBlock
	int numberOfBlock, windingCount, segCount;
	float *jitArray;
	
} geometry_info;

typedef struct {
	size_t numberOfBlock, windingCount, posLength;
	mat4_t *pos;
} geometry_ring;

void alloc_ring(geometry_ring *pRing)
{
	pRing->posLength = pRing->windingCount * pRing->numberOfBlock;
	pRing->pos = (mat4_t*)malloc(pRing->posLength * sizeof(mat4_t));
}

void free_ring(geometry_ring *ring)
{
	free(ring->pos);
}

int generate_ring(float z, float radius, float jit, 
	int windingIndex, geometry_ring *pRing)
{
	float s, c;
	int elementIndex = pRing->numberOfBlock*windingIndex;

	const vec3_t scale = (vec3_t) { 0.65f, 0.05f, 1.0f };

	float angle = 0.0f + jit;
	float step = 360.0f / pRing->numberOfBlock;
	
	for (int i = 0; i < pRing->numberOfBlock; i++) {
		const float angle_in_rad = angle*Deg2Rad;
		lsincosf(angle_in_rad, &s, &c);
		vec3_t offset = (vec3_t) { c * radius, s * radius, z };

		pRing->pos[elementIndex] = get_transform(
			m4_translation(offset),
			m4_rotation_z((angle - 90.0f) * Deg2Rad),
			m4_scaling(scale));

		elementIndex += 1;
		angle += step;
	}
	
	return elementIndex;
}

float move_ring(float deltaZ, geometry_ring *pRing)
{
	//ring->pos[i] = m4_mul(m4_translation((vec3_t) { 0.0f, 0.0f, deltaZ }), ring->pos[i]);
	for (int i = 0; i < pRing->posLength; i++)
		pRing->pos[i].m32 += deltaZ;
	return pRing->pos[0].m32;
}

void draw_ring(const geometry_ring *ring)
{
	for (int i = 0; i < ring->posLength; i++) {
		glPushMatrix();
		glMultMatrixf(&ring->pos[i].m00);
		draw_plane();
		glPopMatrix();
	}
}