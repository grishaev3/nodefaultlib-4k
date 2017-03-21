#pragma once

#include "lmath.h"
#include "math_3d.h"
#include "lmemory.h"
#include "draw.h"

typedef struct {
	//blockCount = 3;	// кол-во блоков в окружности
	//windingCount = 1;		// кол-во слоёв (мотков) с увеличивающимся радиусом
	//segCount=windingCount*blockCount
	uint32_t blockCount, windingCount, posLength;
	mat4_t *pos;
} geometry_ring;

void alloc_ring(geometry_ring *pRing)
{
	pRing->posLength = pRing->windingCount * pRing->blockCount;
	pRing->pos = (mat4_t*)l_malloc(pRing->posLength * sizeof(mat4_t));
}

void free_ring(geometry_ring *ring)
{
	l_free(ring->pos);
}

void generate_ring(float z, float radius, float jit, 
	int windingIndex, geometry_ring *pRing)
{
	float s, c;
	int elementIndex = pRing->blockCount*windingIndex;

	const vec3_t scale = (vec3_t) { 0.65f, 0.125f, 1.0f };

	float angle = 0.0f + jit;
	float step = 360.0f / pRing->blockCount;
	
	for (uint32_t i = 0; i < pRing->blockCount; i++) {
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
}

void generate_ring_winding(float z, float radius, float raduisCoeff, geometry_ring *pRing)
{
	float currentRadius = radius;
	for (uint32_t windingIndex = 0; windingIndex < pRing->windingCount; windingIndex++) {
		float jit = lrandf(-10.0f*windingIndex, 10.0f*windingIndex);
		generate_ring(-z, currentRadius, jit, windingIndex, pRing);
		currentRadius += radius*raduisCoeff;
	}
}

float move_ring(float deltaZ, geometry_ring *pRing)
{
	//ring->pos[i] = m4_mul(m4_translation((vec3_t) { 0.0f, 0.0f, deltaZ }), ring->pos[i]);
	for (uint32_t i = 0; i < pRing->posLength; i++)
		pRing->pos[i].m32 += deltaZ;
	return pRing->pos[0].m32;
}

void draw_ring(const geometry_ring *ring)
{
	for (uint32_t i = 0; i < ring->posLength; i++) {
		//glPushMatrix();
		//glMultMatrixf(&ring->pos[i]);

		lUniformMatrix4fv("u_world", &ring->pos[i]);
		l_draw_plane();
		//glPopMatrix();
	}
}