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

void colorcube()
{
	quad(0, 3, 2, 1);
	quad(2, 3, 7, 6);
	quad(0, 4, 7, 3);
	quad(1, 2, 6, 5);
	quad(4, 5, 6, 7);
	quad(0, 1, 5, 4);
}


//numberOfBlock = 3;	// кол-во блоков в окружности
//windingCount = 1;		// кол-во слоёв (мотков) с увеличивающимся радиусом
//segCount = 10;		// длинна тунеля
typedef struct {
	int numberOfBlock, windingCount, segCount;
	float *jitArray;
	
} geometry_info;

//void fill_jitArray(geometry_info *info)
//{
//	info->jitArray = (float*)LocalAlloc(LPTR,
//		info->segCount * info->windingCount * sizeof(float));
//
//	for (int i = 0; i < info->segCount; i += info->windingCount) {
//		for (int j = 0; j < info->windingCount; j++) {
//			info->jitArray[i + j] = lrandf(10.0f*j, 10.0f*j);
//		}
//	}
//
//}
//void free_jitArray(geometry_info *info)
//{
//	LocalFree(info->jitArray);
//}

typedef struct {
	size_t numberOfBlock, windingCount, posLength;
	mat4_t *pos;
} geometry_ring;

void alloc_ring(geometry_ring *pRing)
{
	pRing->posLength = pRing->windingCount * pRing->numberOfBlock;
	size_t numberOfBytes = pRing->posLength * sizeof(mat4_t);
	pRing->pos = (mat4_t*)malloc(numberOfBytes);
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
	float step = 360 / pRing->numberOfBlock;

	for (float angle = 0.0f + jit; 
		angle < 360.0f - jit; 
		angle += step) {
		const float angle_in_rad = angle*Deg2Rad;
		lsincosf(angle_in_rad, &s, &c);
		vec3_t offset = (vec3_t) { c * radius, s * radius, z };

		mat4_t transform = get_transform(
			m4_translation(offset),
			m4_rotation_z((angle - 90.0f) * Deg2Rad),
			m4_scaling(scale));

		pRing->pos[elementIndex] = transform;
		elementIndex += 1;
	}

	
	return elementIndex;
}

float move_ring(float deltaZ, geometry_ring *ring)
{
	//ring->pos[i] = m4_mul(m4_translation((vec3_t) { 0.0f, 0.0f, deltaZ }), ring->pos[i]);
	for (int i = 0; i < ring->posLength; i++)
		ring->pos[i].m32 += deltaZ;
	return ring->pos[0].m32;
}

void draw_ring(const geometry_ring *ring)
{
	for (int i = 0; i < ring->posLength; i++) {
		glPushMatrix();
		glMultMatrixf((const GLfloat*)&ring->pos[i]);
		colorcube();
		glPopMatrix();
	}
}

/*
void draw_ring(float z, float radius, float jit, const geometry_info *info)
{
	const vec3_t scale = (vec3_t) { 0.65f, 0.05f, 1.0f };

	float step = 360 / info->numberOfBlock;

	for (float angle = 0.0 + jit; angle < 360 - jit; angle += step) {
		const float angle_in_rad = angle*Deg2Rad;

		float s, c;
		lsincosf(angle_in_rad, &s, &c);
		vec3_t offset = (vec3_t) { c * radius, s * radius, z };

		g_transform = get_transform(
			m4_translation(offset),
			m4_rotation_z((angle - 90.0f) * Deg2Rad),
			m4_scaling(scale));

		glPushMatrix();
		glMultMatrixf((const GLfloat*)&g_transform.m); // current * transform
		colorcube();
		glPopMatrix();
	}
}

void draw_geom(const geometry_info *info)
{
	for (int depth = 0; depth < info->segCount; depth += info->windingCount) {
		float z = -(float)depth / info->windingCount;
		float currentRadius = 1.0f;
		for (int w = 0; w < info->windingCount; w++) {
			draw_ring(z, currentRadius, info->jitArray[depth + w], info);
			currentRadius += 0.1f;
		}
	}
}
*/