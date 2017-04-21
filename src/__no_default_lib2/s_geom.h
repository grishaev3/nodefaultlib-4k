#pragma once

#include "lmath.h"
#include "math_3d.h"
#include "lmemory.h"
#include "draw.h"

typedef struct {
	uint32_t blockCount;
	mat4_t *pos;
} geometry_square;

void alloc_square(geometry_square *pSquare)
{
	pSquare->pos = (mat4_t*)l_malloc(pSquare->blockCount * sizeof(mat4_t));
}

void free_square(geometry_square *pSquare)
{
	l_free(pSquare->pos);
}

void generate_square(float radius, geometry_square *pSquare)
{
	mat4_t result;
	int elementIndex = 0;
	float iStep = 12, angle = 0.0f, lengthOfCircum = 2.0f * M_PI*radius;
	const vec3_t scale = (vec3_t) { 0.125f, lengthOfCircum / (float)iStep, 0.2f };
	for (uint32_t i = 0; i < iStep; i++) {

		float s, c; l_sincosf(angle, &s, &c);

		mat4_t translation = m4_translation((vec3_t) { c * radius, s * radius, -(float)j - 0.5f });
		mat4_t rotation = m4_rotation_z(angle);
		mat4_t scalem = m4_scaling(scale);

		get_transform(&result, &translation, &rotation, &scalem);
		pSquare->pos[elementIndex] = result;

		elementIndex += 1;
		angle += (2 * M_PI / iStep);
	}

}

void draw_square()
{
	l_UniformMatrix4fv("u_world", &result, LIT_SHD);
	l_draw_plane();
}
